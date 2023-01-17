// Protocol Buffers - Google's data interchange format
// Copyright 2008 Google Inc.  All rights reserved.
// http://code.google.com/p/protobuf/
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// Author: kenton@google.com (Kenton Varda)
//  Based on original Protocol Buffers design by
//  Sanjay Ghemawat, Jeff Dean, and others.

#include <algorithm>
#include <google/protobuf/stubs/hash.h>
#include <map>
#include <utility>
#include <vector>
#include <google/protobuf/compiler/cpp/cpp_message.h>
#include <google/protobuf/compiler/cpp/cpp_field.h>
#include <google/protobuf/compiler/cpp/cpp_enum.h>
#include <google/protobuf/compiler/cpp/cpp_extension.h>
#include <google/protobuf/compiler/cpp/cpp_helpers.h>
#include <google/protobuf/stubs/strutil.h>
#include <google/protobuf/io/printer.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format.h>
#include <google/protobuf/descriptor.pb.h>


namespace google {
namespace protobuf {
namespace compiler {
namespace cpp {

using internal::WireFormat;
using internal::WireFormatLite;

namespace {

void PrintFieldComment(io::Printer* printer, const FieldDescriptor* field) {
  // Print the field's proto-syntax definition as a comment.  We don't want to
  // print group bodies so we cut off after the first line.
  string def = field->DebugString();
  printer->Print("// $def$\n",
    "def", def.substr(0, def.find_first_of('\n')));
}

struct FieldOrderingByNumber {
  inline bool operator()(const FieldDescriptor* a,
                         const FieldDescriptor* b) const {
    return a->number() < b->number();
  }
};

const char* kWireTypeNames[] = {
  "VARINT",
  "FIXED64",
  "LENGTH_DELIMITED",
  "START_GROUP",
  "END_GROUP",
  "FIXED32",
};

// Sort the fields of the given Descriptor by number into a new[]'d array
// and return it.
const FieldDescriptor** SortFieldsByNumber(const Descriptor* descriptor) {
  const FieldDescriptor** fields =
    new const FieldDescriptor*[descriptor->field_count()];
  for (int i = 0; i < descriptor->field_count(); i++) {
    fields[i] = descriptor->field(i);
  }
  sort(fields, fields + descriptor->field_count(),
       FieldOrderingByNumber());
  return fields;
}

// Functor for sorting extension ranges by their "start" field number.
struct ExtensionRangeSorter {
  bool operator()(const Descriptor::ExtensionRange* left,
                  const Descriptor::ExtensionRange* right) const {
    return left->start < right->start;
  }
};

// Returns true if the "required" restriction check should be ignored for the
// given field.
inline static bool ShouldIgnoreRequiredFieldCheck(
    const FieldDescriptor* field) {
  return false;
}

// Returns true if the message type has any required fields.  If it doesn't,
// we can optimize out calls to its IsInitialized() method.
//
// already_seen is used to avoid checking the same type multiple times
// (and also to protect against recursion).
static bool HasRequiredFields(
    const Descriptor* type,
    hash_set<const Descriptor*>* already_seen) {
  if (already_seen->count(type) > 0) {
    // Since the first occurrence of a required field causes the whole
    // function to return true, we can assume that if the type is already
    // in the cache it didn't have any required fields.
    return false;
  }
  already_seen->insert(type);

  // If the type has extensions, an extension with message type could contain
  // required fields, so we have to be conservative and assume such an
  // extension exists.
  if (type->extension_range_count() > 0) return true;

  for (int i = 0; i < type->field_count(); i++) {
    const FieldDescriptor* field = type->field(i);
    if (field->is_required()) {
      return true;
    }
    if (field->cpp_type() == FieldDescriptor::CPPTYPE_MESSAGE &&
        !ShouldIgnoreRequiredFieldCheck(field)) {
      if (HasRequiredFields(field->message_type(), already_seen)) {
        return true;
      }
    }
  }

  return false;
}

static bool HasRequiredFields(const Descriptor* type) {
  hash_set<const Descriptor*> already_seen;
  return HasRequiredFields(type, &already_seen);
}

// This returns an estimate of the compiler's alignment for the field.  This
// can't guarantee to be correct because the generated code could be compiled on
// different systems with different alignment rules.  The estimates below assume
// 64-bit pointers.
int EstimateAlignmentSize(const FieldDescriptor* field) {
  if (field == NULL) return 0;
  if (field->is_repeated()) return 8;
  switch (field->cpp_type()) {
    case FieldDescriptor::CPPTYPE_BOOL:
      return 1;

    case FieldDescriptor::CPPTYPE_INT32:
    case FieldDescriptor::CPPTYPE_UINT32:
    case FieldDescriptor::CPPTYPE_ENUM:
    case FieldDescriptor::CPPTYPE_FLOAT:
      return 4;

    case FieldDescriptor::CPPTYPE_INT64:
    case FieldDescriptor::CPPTYPE_UINT64:
    case FieldDescriptor::CPPTYPE_DOUBLE:
    case FieldDescriptor::CPPTYPE_STRING:
    case FieldDescriptor::CPPTYPE_MESSAGE:
      return 8;
  }
  GOOGLE_LOG(FATAL) << "Can't get here.";
  return -1;  // Make compiler happy.
}

// FieldGroup is just a helper for OptimizePadding below.  It holds a vector of
// fields that are grouped together because they have compatible alignment, and
// a preferred location in the final field ordering.
class FieldGroup {
 public:
  FieldGroup()
      : preferred_location_(0) {}

  // A group with a single field.
  FieldGroup(float preferred_location, const FieldDescriptor* field)
      : preferred_location_(preferred_location),
        fields_(1, field) {}

  // Append the fields in 'other' to this group.
  void Append(const FieldGroup& other) {
    if (other.fields_.empty()) {
      return;
    }
    // Preferred location is the average among all the fields, so we weight by
    // the number of fields on each FieldGroup object.
    preferred_location_ =
        (preferred_location_ * fields_.size() +
         (other.preferred_location_ * other.fields_.size())) /
        (fields_.size() + other.fields_.size());
    fields_.insert(fields_.end(), other.fields_.begin(), other.fields_.end());
  }

  void SetPreferredLocation(float location) { preferred_location_ = location; }
  const vector<const FieldDescriptor*>& fields() const { return fields_; }

  // FieldGroup objects sort by their preferred location.
  bool operator<(const FieldGroup& other) const {
    return preferred_location_ < other.preferred_location_;
  }

 private:
  // "preferred_location_" is an estimate of where this group should go in the
  // final list of fields.  We compute this by taking the average index of each
  // field in this group in the original ordering of fields.  This is very
  // approximate, but should put this group close to where its member fields
  // originally went.
  float preferred_location_;
  vector<const FieldDescriptor*> fields_;
  // We rely on the default copy constructor and operator= so this type can be
  // used in a vector.
};

// Reorder 'fields' so that if the fields are output into a c++ class in the new
// order, the alignment padding is minimized.  We try to do this while keeping
// each field as close as possible to its original position so that we don't
// reduce cache locality much for function that access each field in order.
void OptimizePadding(vector<const FieldDescriptor*>* fields) {
  // First divide fields into those that align to 1 byte, 4 bytes or 8 bytes.
  vector<FieldGroup> aligned_to_1, aligned_to_4, aligned_to_8;
  for (int i = 0; i < fields->size(); ++i) {
    switch (EstimateAlignmentSize((*fields)[i])) {
      case 1: aligned_to_1.push_back(FieldGroup(i, (*fields)[i])); break;
      case 4: aligned_to_4.push_back(FieldGroup(i, (*fields)[i])); break;
      case 8: aligned_to_8.push_back(FieldGroup(i, (*fields)[i])); break;
      default:
        GOOGLE_LOG(FATAL) << "Unknown alignment size.";
    }
  }

  // Now group fields aligned to 1 byte into sets of 4, and treat those like a
  // single field aligned to 4 bytes.
  for (int i = 0; i < aligned_to_1.size(); i += 4) {
    FieldGroup field_group;
    for (int j = i; j < aligned_to_1.size() && j < i + 4; ++j) {
      field_group.Append(aligned_to_1[j]);
    }
    aligned_to_4.push_back(field_group);
  }
  // Sort by preferred location to keep fields as close to their original
  // location as possible.
  sort(aligned_to_4.begin(), aligned_to_4.end());

  // Now group fields aligned to 4 bytes (or the 4-field groups created above)
  // into pairs, and treat those like a single field aligned to 8 bytes.
  for (int i = 0; i < aligned_to_4.size(); i += 2) {
    FieldGroup field_group;
    for (int j = i; j < aligned_to_4.size() && j < i + 2; ++j) {
      field_group.Append(aligned_to_4[j]);
    }
    if (i == aligned_to_4.size() - 1) {
      // Move incomplete 4-byte block to the end.
      field_group.SetPreferredLocation(fields->size() + 1);
    }
    aligned_to_8.push_back(field_group);
  }
  // Sort by preferred location to keep fields as close to their original
  // location as possible.
  sort(aligned_to_8.begin(), aligned_to_8.end());

  // Now pull out all the FieldDescriptors in order.
  fields->clear();
  for (int i = 0; i < aligned_to_8.size(); ++i) {
    fields->insert(fields->end(),
                   aligned_to_8[i].fields().begin(),
                   aligned_to_8[i].fields().end());
  }
}

}

// ===================================================================

MessageGenerator::MessageGenerator(const Descriptor* descriptor,
                                   const Options& options, bool jsbinding)
  : descriptor_(descriptor),
    classname_(ClassName(descriptor, false)),
    options_(options),
    field_generators_(descriptor, options),
    nested_generators_(new scoped_ptr<MessageGenerator>[
      descriptor->nested_type_count()]),
    enum_generators_(new scoped_ptr<EnumGenerator>[
      descriptor->enum_type_count()]),
    extension_generators_(new scoped_ptr<ExtensionGenerator>[
      descriptor->extension_count()]),
    jsbinding_(jsbinding) {

  for (int i = 0; i < descriptor->nested_type_count(); i++) {
    nested_generators_[i].reset(
      new MessageGenerator(descriptor->nested_type(i), options, jsbinding));
  }

  for (int i = 0; i < descriptor->enum_type_count(); i++) {
    enum_generators_[i].reset(
      new EnumGenerator(descriptor->enum_type(i), options));
  }

  for (int i = 0; i < descriptor->extension_count(); i++) {
    extension_generators_[i].reset(
      new ExtensionGenerator(descriptor->extension(i), options));
  }
}

MessageGenerator::~MessageGenerator() {}

void MessageGenerator::
GenerateForwardDeclaration(io::Printer* printer) {
  printer->Print("class $classname$;\n",
                 "classname", classname_);

  for (int i = 0; i < descriptor_->nested_type_count(); i++) {
    nested_generators_[i]->GenerateForwardDeclaration(printer);
  }
}
    
    void MessageGenerator::
    GeneratePrintDebugInfoFunction(io::Printer* printer) {
        printer->Print("\n");
        printer->Print("void $classname$::printDebugInfo() const \n{\n", "classname", classname_);
        printer->Print("#ifdef DEBUG\n");
        printer->Indent();
        printer->Print("printf(\"$classname$\\n\");\n", "classname", classname_);
        printer->Print("printf(\"\{\\n\");\n");
        for (int i = 0; i < descriptor_->field_count(); i++) {
            const FieldDescriptor* field = descriptor_->field(i);
            string fieldname = FieldName(field);
            if (!field->is_repeated()) {
                if (field->type() == FieldDescriptor::TYPE_MESSAGE) {
                    printer->Print("printf(\"$fieldname$ : \\n\");\n", "fieldname", fieldname);
                    printer->Print("$fieldname$().printDebugInfo();\n", "fieldname", fieldname);
                } else if (field->type() == FieldDescriptor::TYPE_STRING) {
                    printer->Print("printf(\"$fieldname$ : %s\\n\", $fieldname$().c_str());\n", "fieldname", fieldname);
                } else if (field->type() == FieldDescriptor::TYPE_BOOL ||
                           field->type() == FieldDescriptor::TYPE_INT32 ||
                           field->type() == FieldDescriptor::TYPE_UINT32 ||
                           field->type() == FieldDescriptor::TYPE_FIXED32 ||
                           field->type() == FieldDescriptor::TYPE_SFIXED32 ||
                           field->type() == FieldDescriptor::TYPE_SINT32 ||
                           field->type() == FieldDescriptor::TYPE_ENUM) {
                    printer->Print("printf(\"$fieldname$ : %d\\n\", $fieldname$());\n", "fieldname", fieldname);
                } else if (field->type() == FieldDescriptor::TYPE_INT64 ||
                           field->type() == FieldDescriptor::TYPE_SINT64 ||
                           field->type() == FieldDescriptor::TYPE_SFIXED64 ||
                           field->type() == FieldDescriptor::TYPE_FIXED64) {
                    printer->Print("printf(\"$fieldname$ : %ld\\n\", $fieldname$());\n", "fieldname", fieldname);
                } else if (field->type() == FieldDescriptor::TYPE_DOUBLE) {
                    printer->Print("printf(\"$fieldname$ : %lf\\n\", $fieldname$());\n", "fieldname", fieldname);
                } else if (field->type() == FieldDescriptor::TYPE_FLOAT) {
                    printer->Print("printf(\"$fieldname$ : %f\\n\", $fieldname$());\n", "fieldname", fieldname);
                }
            } else {
                printer->Print("printf(\"$fieldname$ : [\\n\");\n", "fieldname", fieldname);
                printer->Print("for(int i = 0; i < $fieldname$_size(); ++i) {\n", "fieldname", fieldname);
                printer->Indent();
                if (field->type() == FieldDescriptor::TYPE_MESSAGE) {
                    printer->Print("$fieldname$(i).printDebugInfo();\n", "fieldname", fieldname);
                } else if (field->type() == FieldDescriptor::TYPE_STRING) {
                    printer->Print("printf(\"%s\\n\", $fieldname$(i).c_str());\n", "fieldname", fieldname);
                } else if (field->type() == FieldDescriptor::TYPE_BOOL ||
                           field->type() == FieldDescriptor::TYPE_INT32 ||
                           field->type() == FieldDescriptor::TYPE_UINT32 ||
                           field->type() == FieldDescriptor::TYPE_FIXED32 ||
                           field->type() == FieldDescriptor::TYPE_SFIXED32 ||
                           field->type() == FieldDescriptor::TYPE_SINT32 ||
                           field->type() == FieldDescriptor::TYPE_ENUM) {
                    printer->Print("printf(\"%d\\n\", $fieldname$(i));\n", "fieldname", fieldname);
                } else if (field->type() == FieldDescriptor::TYPE_INT64 ||
                           field->type() == FieldDescriptor::TYPE_SINT64 ||
                           field->type() == FieldDescriptor::TYPE_SFIXED64 ||
                           field->type() == FieldDescriptor::TYPE_FIXED64) {
                    printer->Print("printf(\"%ld\\n\", $fieldname$(i));\n", "fieldname", fieldname);
                } else if (field->type() == FieldDescriptor::TYPE_DOUBLE) {
                    printer->Print("printf(\"%lf\\n\", $fieldname$(i));\n", "fieldname", fieldname);
                } else if (field->type() == FieldDescriptor::TYPE_FLOAT) {
                    printer->Print("printf(\"%f\\n\", $fieldname$(i));\n", "fieldname", fieldname);
                }
                printer->Outdent();
                printer->Print("}\n");
                printer->Print("printf(\"]\\n\");\n");
            }
        }
        printer->Print("printf(\"}\\n\");\n");
        printer->Outdent();
        printer->Print("#endif\n");
        printer->Print("}");
        printer->Print("\n");
    }

void MessageGenerator::
GenerateEnumDefinitions(io::Printer* printer) {
  for (int i = 0; i < descriptor_->nested_type_count(); i++) {
    nested_generators_[i]->GenerateEnumDefinitions(printer);
  }

  for (int i = 0; i < descriptor_->enum_type_count(); i++) {
    enum_generators_[i]->GenerateDefinition(printer);
  }
}

void MessageGenerator::
GenerateGetEnumDescriptorSpecializations(io::Printer* printer) {
  for (int i = 0; i < descriptor_->nested_type_count(); i++) {
    nested_generators_[i]->GenerateGetEnumDescriptorSpecializations(printer);
  }
  for (int i = 0; i < descriptor_->enum_type_count(); i++) {
    enum_generators_[i]->GenerateGetEnumDescriptorSpecializations(printer);
  }
}

void MessageGenerator::
GenerateFieldAccessorDeclarations(io::Printer* printer) {
  for (int i = 0; i < descriptor_->field_count(); i++) {
    const FieldDescriptor* field = descriptor_->field(i);

    PrintFieldComment(printer, field);

    map<string, string> vars;
    SetCommonFieldVariables(field, &vars, options_);
    vars["constant_name"] = FieldConstantName(field);

    if (field->is_repeated()) {
      printer->Print(vars, "inline int $name$_size() const$deprecation$;\n");
    } else {
      printer->Print(vars, "inline bool has_$name$() const$deprecation$;\n");
    }

    printer->Print(vars, "inline void clear_$name$()$deprecation$;\n");
    printer->Print(vars, "static const int $constant_name$ = $number$;\n");

    // Generate type-specific accessor declarations.
    field_generators_.get(field).GenerateAccessorDeclarations(printer);

    printer->Print("\n");
  }

  if (descriptor_->extension_range_count() > 0) {
    // Generate accessors for extensions.  We just call a macro located in
    // extension_set.h since the accessors about 80 lines of static code.
    printer->Print(
      "GOOGLE_PROTOBUF_EXTENSION_ACCESSORS($classname$)\n",
      "classname", classname_);
  }
}

void MessageGenerator::
GenerateFieldAccessorDefinitions(io::Printer* printer) {
  printer->Print("// $classname$\n\n", "classname", classname_);

  for (int i = 0; i < descriptor_->field_count(); i++) {
    const FieldDescriptor* field = descriptor_->field(i);

    PrintFieldComment(printer, field);

    map<string, string> vars;
    SetCommonFieldVariables(field, &vars, options_);

    // Generate has_$name$() or $name$_size().
    if (field->is_repeated()) {
      printer->Print(vars,
        "inline int $classname$::$name$_size() const {\n"
        "  return $name$_.size();\n"
        "}\n");
    } else {
      // Singular field.
      char buffer[kFastToBufferSize];
      vars["has_array_index"] = SimpleItoa(field->index() / 32);
      vars["has_mask"] = FastHex32ToBuffer(1u << (field->index() % 32), buffer);
      printer->Print(vars,
        "inline bool $classname$::has_$name$() const {\n"
        "  return (_has_bits_[$has_array_index$] & 0x$has_mask$u) != 0;\n"
        "}\n"
        "inline void $classname$::set_has_$name$() {\n"
        "  _has_bits_[$has_array_index$] |= 0x$has_mask$u;\n"
        "}\n"
        "inline void $classname$::clear_has_$name$() {\n"
        "  _has_bits_[$has_array_index$] &= ~0x$has_mask$u;\n"
        "}\n"
        );
    }

    // Generate clear_$name$()
    printer->Print(vars,
      "inline void $classname$::clear_$name$() {\n");

    printer->Indent();
    field_generators_.get(field).GenerateClearingCode(printer);
    printer->Outdent();

    if (!field->is_repeated()) {
      printer->Print(vars,
                     "  clear_has_$name$();\n");
    }

    printer->Print("}\n");

    // Generate type-specific accessors.
    field_generators_.get(field).GenerateInlineAccessorDefinitions(printer);

    printer->Print("\n");
  }
}

void MessageGenerator::
GenerateClassDefinition(io::Printer* printer) {
  for (int i = 0; i < descriptor_->nested_type_count(); i++) {
    nested_generators_[i]->GenerateClassDefinition(printer);
    printer->Print("\n");
    printer->Print(kThinSeparator);
    printer->Print("\n");
  }

  map<string, string> vars;
  vars["classname"] = classname_;
  vars["field_count"] = SimpleItoa(descriptor_->field_count());
  if (options_.dllexport_decl.empty()) {
    vars["dllexport"] = "";
  } else {
    vars["dllexport"] = options_.dllexport_decl + " ";
  }
  vars["superclass"] = SuperClassName(descriptor_);

  printer->Print(vars,
    "class $dllexport$$classname$ : public $superclass$ {\n"
    " public:\n");
  printer->Indent();

  printer->Print(vars,
    "$classname$();\n"
    "virtual ~$classname$();\n"
    "\n"
    "$classname$(const $classname$& from);\n"
    "\n"
    "inline $classname$& operator=(const $classname$& from) {\n"
    "  CopyFrom(from);\n"
    "  return *this;\n"
    "}\n"
    "\n");
    
  printer->Print("void printDebugInfo() const;\n\n");

  if (HasUnknownFields(descriptor_->file())) {
    printer->Print(
      "inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {\n"
      "  return _unknown_fields_;\n"
      "}\n"
      "\n"
      "inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {\n"
      "  return &_unknown_fields_;\n"
      "}\n"
      "\n");
  }

  // Only generate this member if it's not disabled.
  if (HasDescriptorMethods(descriptor_->file()) &&
      !descriptor_->options().no_standard_descriptor_accessor()) {
    printer->Print(vars,
      "static const ::google::protobuf::Descriptor* descriptor();\n");
  }

  printer->Print(vars,
    "static const $classname$& default_instance();\n"
    "\n");

  if (!StaticInitializersForced(descriptor_->file())) {
    printer->Print(vars,
      "#ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER\n"
      "// Returns the internal default instance pointer. This function can\n"
      "// return NULL thus should not be used by the user. This is intended\n"
      "// for Protobuf internal code. Please use default_instance() declared\n"
      "// above instead.\n"
      "static inline const $classname$* internal_default_instance() {\n"
      "  return default_instance_;\n"
      "}\n"
      "#endif\n"
      "\n");
  }


  printer->Print(vars,
    "void Swap($classname$* other);\n"
    "\n"
    "// implements Message ----------------------------------------------\n"
    "\n"
    "$classname$* New() const;\n");

  if (HasGeneratedMethods(descriptor_->file())) {
    if (HasDescriptorMethods(descriptor_->file())) {
      printer->Print(vars,
        "void CopyFrom(const ::google::protobuf::Message& from);\n"
        "void MergeFrom(const ::google::protobuf::Message& from);\n");
    } else {
      printer->Print(vars,
        "void CheckTypeAndMergeFrom(const ::google::protobuf::MessageLite& from);\n");
    }
      
      if (jsbinding_)
      {
          printer->Print("void CopyToJSObject(JSObject* jsobj);\n");
          printer->Print("void CopyFromJSObject(JSObject* jsobj);\n");
          
          printer->Print("virtual jsval ToJSVAL();\n");
      }

    printer->Print(vars,
      "void CopyFrom(const $classname$& from);\n"
      "void MergeFrom(const $classname$& from);\n"
      "void Clear();\n"
      "bool IsInitialized() const;\n"
      "\n"
      "int ByteSize() const;\n"
      "bool MergePartialFromCodedStream(\n"
      "    ::google::protobuf::io::CodedInputStream* input);\n"
      "void SerializeWithCachedSizes(\n"
      "    ::google::protobuf::io::CodedOutputStream* output) const;\n");
    if (HasFastArraySerialization(descriptor_->file())) {
      printer->Print(
        "::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;\n");
    }
  }

  printer->Print(vars,
    "int GetCachedSize() const { return _cached_size_; }\n"
    "private:\n"
    "void SharedCtor();\n"
    "void SharedDtor();\n"
    "void SetCachedSize(int size) const;\n"
    "public:\n"
    "\n");

  if (HasDescriptorMethods(descriptor_->file())) {
    printer->Print(
      "::google::protobuf::Metadata GetMetadata() const;\n"
      "\n");
  } else {
    printer->Print(
      "::std::string GetTypeName() const;\n"
      "\n");
  }

  printer->Print(
    "// nested types ----------------------------------------------------\n"
    "\n");

  // Import all nested message classes into this class's scope with typedefs.
  for (int i = 0; i < descriptor_->nested_type_count(); i++) {
    const Descriptor* nested_type = descriptor_->nested_type(i);
    printer->Print("typedef $nested_full_name$ $nested_name$;\n",
                   "nested_name", nested_type->name(),
                   "nested_full_name", ClassName(nested_type, false));
  }

  if (descriptor_->nested_type_count() > 0) {
    printer->Print("\n");
  }

  // Import all nested enums and their values into this class's scope with
  // typedefs and constants.
  for (int i = 0; i < descriptor_->enum_type_count(); i++) {
    enum_generators_[i]->GenerateSymbolImports(printer);
    printer->Print("\n");
  }

  printer->Print(
    "// accessors -------------------------------------------------------\n"
    "\n");

  // Generate accessor methods for all fields.
  GenerateFieldAccessorDeclarations(printer);

  // Declare extension identifiers.
  for (int i = 0; i < descriptor_->extension_count(); i++) {
    extension_generators_[i]->GenerateDeclaration(printer);
  }


  printer->Print(
    "// @@protoc_insertion_point(class_scope:$full_name$)\n",
    "full_name", descriptor_->full_name());

  // Generate private members.
  printer->Outdent();
  printer->Print(" private:\n");
  printer->Indent();


  for (int i = 0; i < descriptor_->field_count(); i++) {
    if (!descriptor_->field(i)->is_repeated()) {
      printer->Print(
        "inline void set_has_$name$();\n",
        "name", FieldName(descriptor_->field(i)));
      printer->Print(
        "inline void clear_has_$name$();\n",
        "name", FieldName(descriptor_->field(i)));
    }
  }
  printer->Print("\n");

  // To minimize padding, data members are divided into three sections:
  // (1) members assumed to align to 8 bytes
  // (2) members corresponding to message fields, re-ordered to optimize
  //     alignment.
  // (3) members assumed to align to 4 bytes.

  // Members assumed to align to 8 bytes:

  if (descriptor_->extension_range_count() > 0) {
    printer->Print(
      "::google::protobuf::internal::ExtensionSet _extensions_;\n"
      "\n");
  }

  if (HasUnknownFields(descriptor_->file())) {
    printer->Print(
      "::google::protobuf::UnknownFieldSet _unknown_fields_;\n"
      "\n");
  }

  // Field members:

  vector<const FieldDescriptor*> fields;
  for (int i = 0; i < descriptor_->field_count(); i++) {
    fields.push_back(descriptor_->field(i));
  }
  OptimizePadding(&fields);
  for (int i = 0; i < fields.size(); ++i) {
    field_generators_.get(fields[i]).GeneratePrivateMembers(printer);
  }

  // Members assumed to align to 4 bytes:

  // TODO(kenton):  Make _cached_size_ an atomic<int> when C++ supports it.
  printer->Print(
      "\n"
      "mutable int _cached_size_;\n");

  // Generate _has_bits_.
  if (descriptor_->field_count() > 0) {
    printer->Print(vars,
      "::google::protobuf::uint32 _has_bits_[($field_count$ + 31) / 32];\n"
      "\n");
  } else {
    // Zero-size arrays aren't technically allowed, and MSVC in particular
    // doesn't like them.  We still need to declare these arrays to make
    // other code compile.  Since this is an uncommon case, we'll just declare
    // them with size 1 and waste some space.  Oh well.
    printer->Print(
      "::google::protobuf::uint32 _has_bits_[1];\n"
      "\n");
  }

  // Declare AddDescriptors(), BuildDescriptors(), and ShutdownFile() as
  // friends so that they can access private static variables like
  // default_instance_ and reflection_.
  PrintHandlingOptionalStaticInitializers(
    descriptor_->file(), printer,
    // With static initializers.
    "friend void $dllexport_decl$ $adddescriptorsname$();\n",
    // Without.
    "friend void $dllexport_decl$ $adddescriptorsname$_impl();\n",
    // Vars.
    "dllexport_decl", options_.dllexport_decl,
    "adddescriptorsname",
    GlobalAddDescriptorsName(descriptor_->file()->name()));

  printer->Print(
    "friend void $assigndescriptorsname$();\n"
    "friend void $shutdownfilename$();\n"
    "\n",
    "assigndescriptorsname",
      GlobalAssignDescriptorsName(descriptor_->file()->name()),
    "shutdownfilename", GlobalShutdownFileName(descriptor_->file()->name()));

  printer->Print(
    "void InitAsDefaultInstance();\n"
    "static $classname$* default_instance_;\n",
    "classname", classname_);

  printer->Outdent();
  printer->Print(vars, "};");
}
    
    string DotsToUnderscores(const string& name) {
        return StringReplace(name, ".", "_", true);
    }
    
    string DotsToColons(const string& name) {
        return StringReplace(name, ".", "::", true);
    }
    
    string JSClassName(const FieldDescriptor* descriptor) {
        
        // Find "outer", the descriptor of the top-level message in which
        // "descriptor" is embedded.
        const Descriptor* dist = descriptor->message_type();
        const Descriptor* outer = dist;
        while (outer->containing_type() != NULL) outer = outer->containing_type();
        
        const string& outer_name = outer->full_name();
        string packagename = outer_name.substr(0, outer_name.find_last_of("."));
        string classname = "JS"+outer_name.substr(outer_name.find_last_of(".")+1);
        string inner_name = dist->full_name().substr(outer_name.size());
        
        return "::" + DotsToColons(packagename) + "::" + classname + DotsToUnderscores(inner_name);
    }
    
    void MessageGenerator::
    GenerateJSConvertValueDeclaration(io::Printer* printer, const vector<string>& package_parts_) {
        for (int i = 0; i < descriptor_->nested_type_count(); i++) {
            nested_generators_[i]->GenerateJSConvertValueDeclaration(printer, package_parts_);
            printer->Print("\n");
            printer->Print(kThinSeparator);
            printer->Print("\n");
        }
        std::string ns = "::";
        for (int i = 0; i < package_parts_.size(); i++) {
            ns += package_parts_[i] + "::";
        }
        printer->Print("template<>\n");
        printer->Print("jsval value_to_jsval< $ns$$classname$* >($ns$$classname$* value);\n", "ns", ns, "classname", classname_);
    }
    
    void MessageGenerator::
    GenerateJSConvertValueFunction(io::Printer* printer, const vector<string>& package_parts_) {
        for (int i = 0; i < descriptor_->nested_type_count(); i++) {
            nested_generators_[i]->GenerateJSConvertValueFunction(printer, package_parts_);
            printer->Print("\n");
            printer->Print(kThinSeparator);
            printer->Print("\n");
        }
        std::string ns = "::";
        for (int i = 0; i < package_parts_.size(); i++) {
            ns += package_parts_[i] + "::";
        }
        printer->Print("template<>\n");
        printer->Print("jsval value_to_jsval< $ns$$classname$* >($ns$$classname$* value)\n", "ns", ns, "classname", classname_);
        printer->Print("{\n");
        printer->Indent();
        printer->Print("return value_to_jsval1< $ns$JS$classname$, $ns$$classname$>(value);\n", "ns", ns, "classname", classname_);
        printer->Outdent();
        printer->Print("}\n");
    }
    
    void MessageGenerator::
    GenerateJSBindingClassDefinition(io::Printer* printer) {
        for (int i = 0; i < descriptor_->nested_type_count(); i++) {
            nested_generators_[i]->GenerateJSBindingClassDefinition(printer);
            printer->Print("\n");
            printer->Print(kThinSeparator);
            printer->Print("\n");
        }
        
        map<string, string> vars;
        vars["classname"] = "JS" + classname_;
        vars["field_count"] = SimpleItoa(descriptor_->field_count());
        if (options_.dllexport_decl.empty()) {
            vars["dllexport"] = "";
        } else {
            vars["dllexport"] = options_.dllexport_decl + " ";
        }
        vars["superclass"] = "::hoolai::JSCPPWrapper<" + vars["classname"] + ", " + classname_ + ">";
        
        printer->Print(vars,
                       "class $dllexport$$classname$ : public $superclass$ {\n"
                       " public:\n");
        printer->Indent();
        printer->Print("static void jsRegisterClass(JSContext *cx, JSObject *globalObj);\n");
        printer->Print("static JSBool jsCopyToJSObject(JSContext *cx, uint32_t argc, jsval *vp);\n");
        printer->Print("static JSBool jsCopyFromJSObject(JSContext *cx, uint32_t argc, jsval *vp);\n");
        printer->Print("static JSBool jsCopyFrom(JSContext *cx, uint32_t argc, jsval *vp);\n");
        printer->Print("\n");
        
        for (int i = 0; i < descriptor_->field_count(); i++) {
            const FieldDescriptor* field = descriptor_->field(i);
            string fieldname = FieldName(field);
            printer->Print("static JSBool js_$fieldname$(JSContext *cx, uint32_t argc, jsval *vp);\n", "fieldname", fieldname);
            if (field->type() != FieldDescriptor::TYPE_MESSAGE) {
                printer->Print("static JSBool js_set_$fieldname$(JSContext *cx, uint32_t argc, jsval *vp);\n", "fieldname", fieldname);
            }
            if (field->is_repeated()) {
                printer->Print("static JSBool js_$fieldname$_size(JSContext *cx, uint32_t argc, jsval *vp);\n", "fieldname", fieldname);
            } else {
                printer->Print("static JSBool js_has_$fieldname$(JSContext *cx, uint32_t argc, jsval *vp);\n", "fieldname", fieldname);
            }
        }
        
        printer->Outdent();
        printer->Print(vars, "};\n");
    }
    
    void MessageGenerator::
    GenerateJSStaticDefinitions(io::Printer* printer, const vector<string>& package_parts_) {
        for (int i = 0; i < descriptor_->nested_type_count(); i++) {
            nested_generators_[i]->GenerateJSStaticDefinitions(printer, package_parts_);
            printer->Print("\n");
            printer->Print(kThinSeparator);
            printer->Print("\n");
        }
        
        std::string ns = "::";
        for (int i = 0; i < package_parts_.size(); i++) {
            ns += package_parts_[i] + "::";
        }
        map<string, string> vars;
        vars["classname"] = classname_;
        vars["ns"] = ns;
        printer->Print(vars, "template<> JSClass *JSCPPWrapper< $ns$JS$classname$, $ns$$classname$>::jsclass = NULL;\n");
        printer->Print(vars, "template<> JSObject *JSCPPWrapper< $ns$JS$classname$, $ns$$classname$>::proto = NULL;\n");
    }
    
    void MessageGenerator::
    GenerateJSRegisterClassMethod(io::Printer* printer, const vector<string>& package_parts_) {
        for (int i = 0; i < descriptor_->nested_type_count(); i++) {
            nested_generators_[i]->GenerateJSRegisterClassMethod(printer, package_parts_);
            printer->Print("\n");
            printer->Print(kThinSeparator);
            printer->Print("\n");
        }
        
        map<string, string> vars;
        vars["classname"] = classname_;
        printer->Print(vars, "void JS$classname$::jsRegisterClass(JSContext *cx, JSObject *globalObj)\n");
        printer->Print("{\n");
        printer->Indent();
        printer->Print("if (!jsclass)\n{\n");
        printer->Indent();
        printer->Print("jsclass = (JSClass *)calloc(1, sizeof(JSClass));\n");
        printer->Outdent();
        printer->Print("}\n");
        printer->Print(vars, "jsclass->name = \"$classname$\";\n");
        printer->Print("jsclass->addProperty = JS_PropertyStub;\n");
        printer->Print("jsclass->delProperty = JS_DeletePropertyStub;\n");
        printer->Print("jsclass->getProperty = JS_PropertyStub;\n");
        printer->Print("jsclass->setProperty = JS_StrictPropertyStub;\n");
        printer->Print("jsclass->enumerate = JS_EnumerateStub;\n");
        printer->Print("jsclass->resolve = JS_ResolveStub;\n");
        printer->Print("jsclass->convert = JS_ConvertStub;\n");
        printer->Print("jsclass->finalize = jsFinalize;\n");
        printer->Print("jsclass->flags = JSCLASS_HAS_PRIVATE;\n");
        printer->Print("\n");
        printer->Print("static JSPropertySpec properties[] = {\n");
        printer->Indent();
        printer->Print("{0, 0, 0, 0, 0}\n");
        printer->Outdent();
        printer->Print("};\n");
        printer->Print("\n");
        
        printer->Print("static JSFunctionSpec funcs[] = {\n");
        printer->Indent();
        for (int i = 0; i < descriptor_->field_count(); i++) {
            const FieldDescriptor* field = descriptor_->field(i);
            string fieldname = FieldName(field);
            if (field->is_repeated()) {
                printer->Print("JS_FN(\"$fieldname$\", JS$classname$::js_$fieldname$, 1, JSPROP_PERMANENT | JSPROP_READONLY),\n", "fieldname", fieldname, "classname", classname_);
                if (field->type() != FieldDescriptor::TYPE_MESSAGE) {
                    printer->Print("JS_FN(\"set_$fieldname$\", JS$classname$::js_set_$fieldname$, 2, JSPROP_PERMANENT | JSPROP_READONLY),\n", "fieldname", fieldname, "classname", classname_);
                }
                printer->Print("JS_FN(\"$fieldname$_size\", JS$classname$::js_$fieldname$_size, 1, JSPROP_PERMANENT | JSPROP_READONLY),\n", "fieldname", fieldname, "classname", classname_);
            } else {
                printer->Print("JS_FN(\"$fieldname$\", JS$classname$::js_$fieldname$, 0, JSPROP_PERMANENT | JSPROP_READONLY),\n", "fieldname", fieldname, "classname", classname_);
                if (field->type() != FieldDescriptor::TYPE_MESSAGE) {
                    printer->Print("JS_FN(\"set_$fieldname$\", JS$classname$::js_set_$fieldname$, 1, JSPROP_PERMANENT | JSPROP_READONLY),\n", "fieldname", fieldname, "classname", classname_);
                }
                printer->Print("JS_FN(\"has_$fieldname$\", JS$classname$::js_has_$fieldname$, 0, JSPROP_PERMANENT | JSPROP_READONLY),\n", "fieldname", fieldname, "classname", classname_);
            }
            
        }
        printer->Print("JS_FN(\"copyToJSObject\", JS$classname$::jsCopyToJSObject, 1, JSPROP_PERMANENT | JSPROP_READONLY),\n", "classname", classname_);
        printer->Print("JS_FN(\"copyFrom\", JS$classname$::jsCopyFrom, 1, JSPROP_PERMANENT | JSPROP_READONLY),\n", "classname", classname_);
        printer->Print("JS_FN(\"copyFromJSObject\", JS$classname$::jsCopyFromJSObject, 1, JSPROP_PERMANENT | JSPROP_READONLY),\n", "classname", classname_);
        printer->Print("JS_FS_END\n");
        printer->Outdent();
        printer->Print("};\n");
        printer->Print("\n");
        
        printer->Print("static JSFunctionSpec st_funcs[] = {\n");
        printer->Indent();
        printer->Print("JS_FS_END\n");
        printer->Outdent();
        printer->Print("};\n");
        printer->Print("\n");
        printer->Print("JSObject *tmp = globalObj;\n");
        if (package_parts_.size() > 0) {
            printer->Print("JSObject *tmp1 = globalObj;\n");
            printer->Print("JSBool hasprop;\n");
            printer->Print("JS::RootedValue val(cx);\n");
            for (int i = 0; i < package_parts_.size(); ++i) {
                printer->Print("JS_HasProperty(cx, tmp, \"$name$\", &hasprop);\n", "name", package_parts_[i]);
                printer->Print("if (hasprop) {\n");
                printer->Indent();
                printer->Print("JS_GetProperty(cx, tmp, \"$name$\", &val);\n", "name", package_parts_[i]);
                printer->Print("tmp1 = val.toObjectOrNull();\n");
                printer->Outdent();
                printer->Print("} else {\n");
                printer->Indent();
                printer->Print("tmp1 = JS_NewObject(cx, NULL, NULL, NULL);\n");
                printer->Print("val.setObjectOrNull(tmp1);\n");
                printer->Outdent();
                printer->Print("}\n");
                printer->Print("JS_SetProperty(cx, tmp, \"$name$\", val);\n", "name", package_parts_[i]);
                printer->Print("tmp = tmp1;\n");
            }
        }
        printer->Print("proto = JS_InitClass(cx,tmp,NULL,jsclass,::hoolai::jsNoConstructor,0,properties,funcs,NULL,st_funcs);\n", "classname", classname_);
        
        printer->Outdent();
        printer->Print("}\n");
    }
    
    void MessageGenerator::
    GenerateJSBindingMethods(io::Printer* printer) {
        for (int i = 0; i < descriptor_->nested_type_count(); i++) {
            nested_generators_[i]->GenerateJSBindingMethods(printer);
            printer->Print("\n");
            printer->Print(kThinSeparator);
            printer->Print("\n");
        }
        
        printer->Print("JSBool JS$classname$::jsCopyToJSObject(JSContext *cx, uint32_t argc, jsval *vp)\n", "classname", classname_);
        printer->Print("{\n");
        printer->Indent();
        printer->Print("if (argc == 1)\n");
        printer->Print("{\n");
        printer->Indent();
        printer->Print("JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);\n");
        printer->Print("JS$classname$* thiz = (JS$classname$*)JS_GetPrivate(obj);\n", "classname", classname_);
        printer->Print("JSObject *arg0;\n");
        printer->Print("JS_ConvertArguments(cx, 1, JS_ARGV(cx, vp), \"o\", &arg0);\n");
        printer->Print("thiz->getNativeObject()->CopyToJSObject(arg0);\n");
        printer->Print("JS_SET_RVAL(cx, vp, JSVAL_VOID);\n");
        printer->Print("return JS_TRUE;\n");
        printer->Outdent();
        printer->Print("}\n");
        printer->Print("return JS_FALSE;\n");
        printer->Outdent();
        printer->Print("}\n");
        printer->Print("\n");
        
        printer->Print("JSBool JS$classname$::jsCopyFrom(JSContext *cx, uint32_t argc, jsval *vp)\n", "classname", classname_);
        printer->Print("{\n");
        printer->Indent();
        printer->Print("if (argc == 1)\n");
        printer->Print("{\n");
        printer->Indent();
        printer->Print("JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);\n");
        printer->Print("JS$classname$* thiz = (JS$classname$*)JS_GetPrivate(obj);\n", "classname", classname_);
        printer->Print("JSObject *arg0;\n");
        printer->Print("JS_ConvertArguments(cx, 1, JS_ARGV(cx, vp), \"o\", &arg0);\n");
        printer->Print("JS$classname$ *wrapper = (JS$classname$ *)JS_GetPrivate(arg0);\n", "classname", classname_);
        printer->Print("thiz->getNativeObject()->CopyFrom(*(wrapper->getNativeObject()));\n");
        printer->Print("JS_SET_RVAL(cx, vp, JSVAL_VOID);\n");
        printer->Print("return JS_TRUE;\n");
        printer->Outdent();
        printer->Print("}\n");
        printer->Print("return JS_FALSE;\n");
        printer->Outdent();
        printer->Print("}\n");
        printer->Print("\n");
        
        printer->Print("JSBool JS$classname$::jsCopyFromJSObject(JSContext *cx, uint32_t argc, jsval *vp)\n", "classname", classname_);
        printer->Print("{\n");
        printer->Indent();
        printer->Print("if (argc == 1)\n");
        printer->Print("{\n");
        printer->Indent();
        printer->Print("JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);\n");
        printer->Print("JS$classname$* thiz = (JS$classname$*)JS_GetPrivate(obj);\n", "classname", classname_);
        printer->Print("JSObject *arg0;\n");
        printer->Print("JS_ConvertArguments(cx, 1, JS_ARGV(cx, vp), \"o\", &arg0);\n");
        printer->Print("thiz->getNativeObject()->CopyFromJSObject(arg0);\n");
        printer->Print("JS_SET_RVAL(cx, vp, JSVAL_VOID);\n");
        printer->Print("return JS_TRUE;\n");
        printer->Outdent();
        printer->Print("}\n");
        printer->Print("return JS_FALSE;\n");
        printer->Outdent();
        printer->Print("}\n");
        printer->Print("\n");
        
        for (int i = 0; i < descriptor_->field_count(); i++) {
            const FieldDescriptor* field = descriptor_->field(i);
            string fieldname = FieldName(field);
            if (field->is_repeated()) {
                printer->Print("JSBool JS$classname$::js_$fieldname$(JSContext *cx, uint32_t argc, jsval *vp)\n", "fieldname", fieldname, "classname", classname_);
                printer->Print("{\n");
                printer->Indent();
                printer->Print("JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);\n");
                printer->Print("JS$classname$* thiz = (JS$classname$*)JS_GetPrivate(obj);\n", "classname", classname_);
                
                printer->Print("if (argc == 0)\n");
                printer->Print("{\n");
                printer->Indent();
                
                printer->Print("int size = (int)thiz->getNativeObject()->$fieldname$_size();\n", "fieldname", fieldname);
                printer->Print("jsval *resarr = new jsval[size];\n");
                printer->Print("for (int i = 0; i < size; ++i)\n");
                printer->Print("{\n");
                printer->Indent();
                if (field->type() == FieldDescriptor::TYPE_MESSAGE) {
                    printer->Print("$classname$* res = $classname$::getOrCreateWrapper(cx, thiz->getNativeObject()->mutable_$fieldname$(i));\n", "fieldname", fieldname, "classname", JSClassName(field));
                    printer->Print("resarr[i] = OBJECT_TO_JSVAL(res->jsobject);\n");
                } else if (field->type() == FieldDescriptor::TYPE_STRING) {
                    printer->Print("resarr[i] = ::hoolai::value_to_jsval(thiz->getNativeObject()->$fieldname$(i).c_str());\n", "fieldname", fieldname);
                } else if (field->type() == FieldDescriptor::TYPE_BOOL) {
                    printer->Print("resarr[i] = BOOLEAN_TO_JSVAL(thiz->getNativeObject()->$fieldname$(i));\n", "fieldname", fieldname);
                } else if (field->type() == FieldDescriptor::TYPE_INT32 ||
                           field->type() == FieldDescriptor::TYPE_FIXED32 ||
                           field->type() == FieldDescriptor::TYPE_SFIXED32 ||
                           field->type() == FieldDescriptor::TYPE_SINT32 ||
                           field->type() == FieldDescriptor::TYPE_ENUM) {
                    printer->Print("resarr[i] = INT_TO_JSVAL(thiz->getNativeObject()->$fieldname$(i));\n", "fieldname", fieldname);
                } else if (field->type() == FieldDescriptor::TYPE_UINT32) {
                    printer->Print("resarr[i] = UINT_TO_JSVAL(thiz->getNativeObject()->$fieldname$(i));\n", "fieldname", fieldname);
                } else if (field->type() == FieldDescriptor::TYPE_DOUBLE) {
                    printer->Print("resarr[i] = DOUBLE_TO_JSVAL(thiz->getNativeObject()->$fieldname$(i));\n", "fieldname", fieldname);
                } else if (field->type() == FieldDescriptor::TYPE_FLOAT) {
                    printer->Print("resarr[i] = DOUBLE_TO_JSVAL(thiz->getNativeObject()->$fieldname$(i));\n", "fieldname", fieldname);
                } else if (field->type() == FieldDescriptor::TYPE_INT64 ||
                           field->type() == FieldDescriptor::TYPE_UINT64) {
                    printer->Print("resarr[i] = DOUBLE_TO_JSVAL(thiz->getNativeObject()->$fieldname$(i));\n", "fieldname", fieldname);
                } else if (field->type() == FieldDescriptor::TYPE_BYTES) {
                    printer->Print("void * contents = NULL;\n");
                    printer->Print("uint8_t * data = NULL;\n");
                    printer->Print("const std::string &value = thiz->getNativeObject()->$fieldname$(i);\n", "fieldname", fieldname);
                    printer->Print("if (JS_AllocateArrayBufferContents(cx, (uint32_t)value.length(), &contents, &data)) {\n");
                    printer->Indent();
                    printer->Print("memcpy(data, value.c_str(), value.length());\n");
                    printer->Print("resarr[i] = OBJECT_TO_JSVAL(JS_NewArrayBufferWithContents(cx, contents));\n");
                    printer->Outdent();
                    printer->Print("} else {resarr[i] = JSVAL_NULL;}\n");
                } else {
                    printer->Print("delete[] resarr;\n");
                    printer->Print("return JS_FALSE;\n");
                }
                printer->Outdent();
                printer->Print("}\n");
                printer->Print("JSObject *tmp = JS_NewArrayObject(cx, size, resarr);\n");
                printer->Print("JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(tmp));\n");
                printer->Print("delete[] resarr;\n");
                printer->Print("return JS_TRUE;\n");
                
                printer->Outdent();
                printer->Print("}\n");
                
                printer->Print("else if (argc == 1)\n");
                printer->Print("{\n");
                printer->Indent();
                
                printer->Print("int arg0;\n");
                printer->Print("JS_ConvertArguments(cx, 1, JS_ARGV(cx, vp), \"i\", &arg0);\n");
                
                if (field->type() == FieldDescriptor::TYPE_MESSAGE) {
                    printer->Print("$classname$* res = $classname$::getOrCreateWrapper(cx, thiz->getNativeObject()->mutable_$fieldname$(arg0));\n", "fieldname", fieldname, "classname", JSClassName(field));
                    printer->Print("JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(res->jsobject));\n");
                    printer->Print("return JS_TRUE;\n");
                } else if (field->type() == FieldDescriptor::TYPE_STRING) {
                    printer->Print("JS_SET_RVAL(cx, vp, ::hoolai::value_to_jsval(thiz->getNativeObject()->$fieldname$(arg0).c_str()));\n", "fieldname", fieldname);
                    printer->Print("return JS_TRUE;\n");
                } else if (field->type() == FieldDescriptor::TYPE_BOOL) {
                    printer->Print("JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(thiz->getNativeObject()->$fieldname$(arg0)));\n", "fieldname", fieldname);
                    printer->Print("return JS_TRUE;\n");
                } else if (field->type() == FieldDescriptor::TYPE_INT32 ||
                           field->type() == FieldDescriptor::TYPE_FIXED32 ||
                           field->type() == FieldDescriptor::TYPE_SFIXED32 ||
                           field->type() == FieldDescriptor::TYPE_SINT32 ||
                           field->type() == FieldDescriptor::TYPE_ENUM) {
                    printer->Print("JS_SET_RVAL(cx, vp, INT_TO_JSVAL(thiz->getNativeObject()->$fieldname$(arg0)));\n", "fieldname", fieldname);
                    printer->Print("return JS_TRUE;\n");
                } else if (field->type() == FieldDescriptor::TYPE_UINT32) {
                    printer->Print("JS_SET_RVAL(cx, vp, UINT_TO_JSVAL(thiz->getNativeObject()->$fieldname$(arg0)));\n", "fieldname", fieldname);
                    printer->Print("return JS_TRUE;\n");
                } else if (field->type() == FieldDescriptor::TYPE_DOUBLE ||
                           field->type() == FieldDescriptor::TYPE_INT64 ||
                           field->type() == FieldDescriptor::TYPE_UINT64) {
                    printer->Print("JS_SET_RVAL(cx, vp, DOUBLE_TO_JSVAL(thiz->getNativeObject()->$fieldname$(arg0)));\n", "fieldname", fieldname);
                    printer->Print("return JS_TRUE;\n");
                } else if (field->type() == FieldDescriptor::TYPE_FLOAT) {
                    printer->Print("JS_SET_RVAL(cx, vp, DOUBLE_TO_JSVAL(thiz->getNativeObject()->$fieldname$(arg0)));\n", "fieldname", fieldname);
                    printer->Print("return JS_TRUE;\n");
                } else if (field->type() == FieldDescriptor::TYPE_BYTES) {
                    printer->Print("void * contents = NULL;\n");
                    printer->Print("uint8_t * data = NULL;\n");
                    printer->Print("const std::string &value = thiz->getNativeObject()->$fieldname$(arg0);\n", "fieldname", fieldname);
                    printer->Print("if (JS_AllocateArrayBufferContents(cx, (uint32_t)value.length(), &contents, &data)) {\n");
                    printer->Indent();
                    printer->Print("memcpy(data, value.c_str(), value.length());\n");
                    printer->Print("JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(JS_NewArrayBufferWithContents(cx, contents)));\n");
                    printer->Outdent();
                    printer->Print("} else {JS_SET_RVAL(cx, vp, JSVAL_NULL);}\n");
                    printer->Print("return JS_TRUE;\n");
                }
                
                printer->Outdent();
                printer->Print("}\n");
                printer->Print("return JS_FALSE;\n");
                printer->Outdent();
                printer->Print("}\n");
                printer->Print("\n");
                
                if (field->type() != FieldDescriptor::TYPE_MESSAGE) {
                    printer->Print("JSBool JS$classname$::js_set_$fieldname$(JSContext *cx, uint32_t argc, jsval *vp)\n", "fieldname", fieldname, "classname", classname_);
                    printer->Print("{\n");
                    printer->Indent();
                    printer->Print("JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);\n");
                    printer->Print("JS$classname$* thiz = (JS$classname$*)JS_GetPrivate(obj);\n", "classname", classname_);
                    printer->Print("if (argc == 2)\n");
                    printer->Print("{\n");
                    printer->Indent();
                    if (field->type() == FieldDescriptor::TYPE_STRING) {
                        printer->Print("JSString *arg0; int arg1;\n");
                        printer->Print("JS_ConvertArguments(cx, 2, JS_ARGV(cx, vp), \"iS\", &arg1, &arg0);\n");
                        printer->Print("char *s = JS_EncodeStringToUTF8(cx, arg0);\n");
                        printer->Print("thiz->getNativeObject()->set_$fieldname$(arg1, s);\n", "fieldname", fieldname);
                        printer->Print("JS_free(cx, s);\n");
                        printer->Print("\n");
                        printer->Print("JS_SET_RVAL(cx, vp, JSVAL_VOID);\n");
                        printer->Print("return JS_TRUE;\n");
                    } else if (field->type() == FieldDescriptor::TYPE_BOOL) {
                        printer->Print("JSBool arg0; int arg1;\n");
                        printer->Print("JS_ConvertArguments(cx, 2, JS_ARGV(cx, vp), \"ib\", &arg1, &arg0);\n");
                        printer->Print("thiz->getNativeObject()->set_$fieldname$(arg1, arg0);\n", "fieldname", fieldname);
                        printer->Print("\n");
                        printer->Print("JS_SET_RVAL(cx, vp, JSVAL_VOID);\n");
                        printer->Print("return JS_TRUE;\n");
                    } else if (field->type() == FieldDescriptor::TYPE_INT32 ||
                               field->type() == FieldDescriptor::TYPE_FIXED32 ||
                               field->type() == FieldDescriptor::TYPE_SFIXED32 ||
                               field->type() == FieldDescriptor::TYPE_SINT32 ||
                               field->type() == FieldDescriptor::TYPE_ENUM) {
                        printer->Print("int32_t arg0; int arg1;\n");
                        printer->Print("JS_ConvertArguments(cx, 2, JS_ARGV(cx, vp), \"ii\", &arg1, &arg0);\n");
                        printer->Print("thiz->getNativeObject()->set_$fieldname$(arg1, arg0);\n", "fieldname", fieldname);
                        printer->Print("\n");
                        printer->Print("JS_SET_RVAL(cx, vp, JSVAL_VOID);\n");
                        printer->Print("return JS_TRUE;\n");
                    } else if(field->type() == FieldDescriptor::TYPE_UINT32) {
                        printer->Print("uint32_t arg0; int arg1;\n");
                        printer->Print("JS_ConvertArguments(cx, 2, JS_ARGV(cx, vp), \"iu\", &arg1, &arg0);\n");
                        printer->Print("thiz->getNativeObject()->set_$fieldname$(arg1, arg0);\n", "fieldname", fieldname);
                        printer->Print("\n");
                        printer->Print("JS_SET_RVAL(cx, vp, JSVAL_VOID);\n");
                        printer->Print("return JS_TRUE;\n");
                    } else if (field->type() == FieldDescriptor::TYPE_DOUBLE || field->type() == FieldDescriptor::TYPE_FLOAT) {
                        printer->Print("double arg0; int arg1;\n");
                        printer->Print("JS_ConvertArguments(cx, 2, JS_ARGV(cx, vp), \"id\", &arg1, &arg0);\n");
                        printer->Print("thiz->getNativeObject()->set_$fieldname$(arg1, arg0);\n", "fieldname", fieldname);
                        printer->Print("\n");
                        printer->Print("JS_SET_RVAL(cx, vp, JSVAL_VOID);\n");
                        printer->Print("return JS_TRUE;\n");
                    } else if (field->type() == FieldDescriptor::TYPE_INT64) {
                        printer->Print("double arg0; int arg1;\n");
                        printer->Print("JS_ConvertArguments(cx, 2, JS_ARGV(cx, vp), \"id\", &arg1, &arg0);\n");
                        printer->Print("thiz->getNativeObject()->set_$fieldname$((int64_t)arg1, arg0);\n", "fieldname", fieldname);
                        printer->Print("\n");
                        printer->Print("JS_SET_RVAL(cx, vp, JSVAL_VOID);\n");
                        printer->Print("return JS_TRUE;\n");
                    } else if (field->type() == FieldDescriptor::TYPE_UINT64) {
                        printer->Print("double arg0; int arg1;\n");
                        printer->Print("JS_ConvertArguments(cx, 2, JS_ARGV(cx, vp), \"id\", &arg1, &arg0);\n");
                        printer->Print("thiz->getNativeObject()->set_$fieldname$((uint64_t)arg1, arg0);\n", "fieldname", fieldname);
                        printer->Print("\n");
                        printer->Print("JS_SET_RVAL(cx, vp, JSVAL_VOID);\n");
                        printer->Print("return JS_TRUE;\n");
                    } else if (field->type() == FieldDescriptor::TYPE_BYTES) {
                        printer->Print("JSObject *arg0; int arg1;\n");
                        printer->Print("JS_ConvertArguments(cx, 2, JS_ARGV(cx, vp), \"io\", &arg1, &arg0);\n");
                        printer->Print("uint32_t len = JS_GetArrayBufferByteLength(arg0);\n");
                        printer->Print("uint8_t *data = JS_GetArrayBufferData(arg0);\n");
                        printer->Print("std::string s((const char*)data, len);\n");
                        printer->Print("thiz->getNativeObject()->set_$fieldname$(arg1, s);\n", "fieldname", fieldname);
                        printer->Print("\n");
                        printer->Print("JS_SET_RVAL(cx, vp, JSVAL_VOID);\n");
                        printer->Print("return JS_TRUE;\n");
                    }
                    printer->Outdent();
                    printer->Print("}\n");
                    printer->Print("return JS_FALSE;\n");
                    printer->Outdent();
                    printer->Print("}\n");
                    printer->Print("\n");
                }
                
                printer->Print("JSBool JS$classname$::js_$fieldname$_size(JSContext *cx, uint32_t argc, jsval *vp)\n", "fieldname", fieldname, "classname", classname_);
                printer->Print("{\n");
                printer->Indent();
                printer->Print("JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);\n");
                printer->Print("JS$classname$* thiz = (JS$classname$*)JS_GetPrivate(obj);\n", "classname", classname_);
                printer->Print("JS_SET_RVAL(cx, vp, INT_TO_JSVAL(thiz->getNativeObject()->$fieldname$_size()));\n", "fieldname", fieldname);
                printer->Print("\n");
                printer->Print("return JS_TRUE;\n");
                printer->Outdent();
                printer->Print("}\n");
                printer->Print("\n");
            } else {
                printer->Print("JSBool JS$classname$::js_$fieldname$(JSContext *cx, uint32_t argc, jsval *vp)\n", "fieldname", fieldname, "classname", classname_);
                printer->Print("{\n");
                printer->Indent();
                printer->Print("JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);\n");
                printer->Print("JS$classname$* thiz = (JS$classname$*)JS_GetPrivate(obj);\n", "classname", classname_);
                if (field->type() == FieldDescriptor::TYPE_MESSAGE) {
                    printer->Print("$classname$* res = $classname$::getOrCreateWrapper(cx, thiz->getNativeObject()->mutable_$fieldname$());\n", "fieldname", fieldname, "classname", JSClassName(field));
                    printer->Print("JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(res->jsobject));\n");
                    printer->Print("return JS_TRUE;\n");
                } else if (field->type() == FieldDescriptor::TYPE_STRING) {
                    printer->Print("JS_SET_RVAL(cx, vp, ::hoolai::value_to_jsval(thiz->getNativeObject()->$fieldname$().c_str()));\n", "fieldname", fieldname);
                    printer->Print("return JS_TRUE;\n");
                } else if (field->type() == FieldDescriptor::TYPE_BOOL) {
                    printer->Print("JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(thiz->getNativeObject()->$fieldname$()));\n", "fieldname", fieldname);
                    printer->Print("return JS_TRUE;\n");
                } else if (field->type() == FieldDescriptor::TYPE_INT32 ||
                           field->type() == FieldDescriptor::TYPE_FIXED32 ||
                           field->type() == FieldDescriptor::TYPE_SFIXED32 ||
                           field->type() == FieldDescriptor::TYPE_SINT32 ||
                           field->type() == FieldDescriptor::TYPE_ENUM) {
                    printer->Print("JS_SET_RVAL(cx, vp, INT_TO_JSVAL(thiz->getNativeObject()->$fieldname$()));\n", "fieldname", fieldname);
                    printer->Print("return JS_TRUE;\n");
                } else if (field->type() == FieldDescriptor::TYPE_UINT32) {
                    printer->Print("JS_SET_RVAL(cx, vp, UINT_TO_JSVAL(thiz->getNativeObject()->$fieldname$()));\n", "fieldname", fieldname);
                    printer->Print("return JS_TRUE;\n");
                } else if (field->type() == FieldDescriptor::TYPE_DOUBLE ||
                           field->type() == FieldDescriptor::TYPE_INT64 ||
                           field->type() == FieldDescriptor::TYPE_UINT64) {
                    printer->Print("JS_SET_RVAL(cx, vp, DOUBLE_TO_JSVAL(thiz->getNativeObject()->$fieldname$()));\n", "fieldname", fieldname);
                    printer->Print("return JS_TRUE;\n");
                } else if (field->type() == FieldDescriptor::TYPE_FLOAT) {
                    printer->Print("JS_SET_RVAL(cx, vp, DOUBLE_TO_JSVAL(thiz->getNativeObject()->$fieldname$()));\n", "fieldname", fieldname);
                    printer->Print("return JS_TRUE;\n");
                } else if (field->type() == FieldDescriptor::TYPE_BYTES) {
                    printer->Print("void * contents = NULL;\n");
                    printer->Print("uint8_t * data = NULL;\n");
                    printer->Print("const std::string &value = thiz->getNativeObject()->$fieldname$();\n", "fieldname", fieldname);
                    printer->Print("if (JS_AllocateArrayBufferContents(cx, (uint32_t)value.length(), &contents, &data)) {\n");
                    printer->Indent();
                    printer->Print("memcpy(data, value.c_str(), value.length());\n");
                    printer->Print("JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(JS_NewArrayBufferWithContents(cx, contents)));\n");
                    printer->Outdent();
                    printer->Print("} else {JS_SET_RVAL(cx, vp, JSVAL_NULL);}\n");
                    printer->Print("return JS_TRUE;\n");
                } else {
                    printer->Print("return JS_FALSE;\n");
                }
                printer->Outdent();
                printer->Print("}\n");
                printer->Print("\n");
                
                if (field->type() != FieldDescriptor::TYPE_MESSAGE) {
                    printer->Print("JSBool JS$classname$::js_set_$fieldname$(JSContext *cx, uint32_t argc, jsval *vp)\n", "fieldname", fieldname, "classname", classname_);
                    printer->Print("{\n");
                    printer->Indent();
                    printer->Print("JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);\n");
                    printer->Print("JS$classname$* thiz = (JS$classname$*)JS_GetPrivate(obj);\n", "classname", classname_);
                    printer->Print("if (argc == 1)\n");
                    printer->Print("{\n");
                    printer->Indent();
                    if (field->type()) {
                        if (field->type() == FieldDescriptor::TYPE_STRING) {
                            printer->Print("JSString *arg0;\n");
                            printer->Print("JS_ConvertArguments(cx, 1, JS_ARGV(cx, vp), \"S\", &arg0);\n");
                            printer->Print("char* s = JS_EncodeStringToUTF8(cx, arg0);\n");
                            printer->Print("thiz->getNativeObject()->set_$fieldname$(s);\n", "fieldname", fieldname);
                            printer->Print("JS_free(cx, s);\n");
                            printer->Print("\n");
                            printer->Print("JS_SET_RVAL(cx, vp, JSVAL_VOID);\n");
                            printer->Print("return JS_TRUE;\n");
                        } else if (field->type() == FieldDescriptor::TYPE_BOOL) {
                            printer->Print("JSBool arg0;\n");
                            printer->Print("JS_ConvertArguments(cx, 1, JS_ARGV(cx, vp), \"b\", &arg0);\n");
                            printer->Print("thiz->getNativeObject()->set_$fieldname$(arg0);\n", "fieldname", fieldname);
                            printer->Print("\n");
                            printer->Print("JS_SET_RVAL(cx, vp, JSVAL_VOID);\n");
                            printer->Print("return JS_TRUE;\n");
                        } else if (field->type() == FieldDescriptor::TYPE_INT32 ||
                                   field->type() == FieldDescriptor::TYPE_FIXED32 ||
                                   field->type() == FieldDescriptor::TYPE_SFIXED32 ||
                                   field->type() == FieldDescriptor::TYPE_SINT32 ||
                                   field->type() == FieldDescriptor::TYPE_ENUM) {
                            printer->Print("int32_t arg0;\n");
                            printer->Print("JS_ConvertArguments(cx, 1, JS_ARGV(cx, vp), \"i\", &arg0);\n");
                            printer->Print("thiz->getNativeObject()->set_$fieldname$(arg0);\n", "fieldname", fieldname);
                            printer->Print("\n");
                            printer->Print("JS_SET_RVAL(cx, vp, JSVAL_VOID);\n");
                            printer->Print("return JS_TRUE;\n");
                        } else if(field->type() == FieldDescriptor::TYPE_UINT32) {
                            printer->Print("uint32_t arg0;\n");
                            printer->Print("JS_ConvertArguments(cx, 1, JS_ARGV(cx, vp), \"u\", &arg0);\n");
                            printer->Print("thiz->getNativeObject()->set_$fieldname$(arg0);\n", "fieldname", fieldname);
                            printer->Print("\n");
                            printer->Print("JS_SET_RVAL(cx, vp, JSVAL_VOID);\n");
                            printer->Print("return JS_TRUE;\n");
                        } else if (field->type() == FieldDescriptor::TYPE_DOUBLE || field->type() == FieldDescriptor::TYPE_FLOAT) {
                            printer->Print("double arg0;\n");
                            printer->Print("JS_ConvertArguments(cx, 1, JS_ARGV(cx, vp), \"d\", &arg0);\n");
                            printer->Print("thiz->getNativeObject()->set_$fieldname$(arg0);\n", "fieldname", fieldname);
                            printer->Print("\n");
                            printer->Print("JS_SET_RVAL(cx, vp, JSVAL_VOID);\n");
                            printer->Print("return JS_TRUE;\n");
                        } else if (field->type() == FieldDescriptor::TYPE_INT64) {
                            printer->Print("double arg0;\n");
                            printer->Print("JS_ConvertArguments(cx, 1, JS_ARGV(cx, vp), \"d\", &arg0);\n");
                            printer->Print("thiz->getNativeObject()->set_$fieldname$((int64_t)arg0);\n", "fieldname", fieldname);
                            printer->Print("\n");
                            printer->Print("JS_SET_RVAL(cx, vp, JSVAL_VOID);\n");
                            printer->Print("return JS_TRUE;\n");
                        } else if (field->type() == FieldDescriptor::TYPE_UINT64) {
                            printer->Print("double arg0;\n");
                            printer->Print("JS_ConvertArguments(cx, 1, JS_ARGV(cx, vp), \"d\", &arg0);\n");
                            printer->Print("thiz->getNativeObject()->set_$fieldname$((uint64_t)arg0);\n", "fieldname", fieldname);
                            printer->Print("\n");
                            printer->Print("JS_SET_RVAL(cx, vp, JSVAL_VOID);\n");
                            printer->Print("return JS_TRUE;\n");
                        } else if (field->type() == FieldDescriptor::TYPE_BYTES) {
                            printer->Print("JSObject *arg0;\n");
                            printer->Print("JS_ConvertArguments(cx, 2, JS_ARGV(cx, vp), \"o\", &arg0);\n");
                            printer->Print("uint32_t len = JS_GetArrayBufferByteLength(arg0);\n");
                            printer->Print("uint8_t *data = JS_GetArrayBufferData(arg0);\n");
                            printer->Print("std::string s((const char*)data, len);\n");
                            printer->Print("thiz->getNativeObject()->set_$fieldname$(s);\n", "fieldname", fieldname);
                            printer->Print("\n");
                            printer->Print("JS_SET_RVAL(cx, vp, JSVAL_VOID);\n");
                            printer->Print("return JS_TRUE;\n");
                        }
                    }
                    printer->Outdent();
                    printer->Print("}\n");
                    printer->Print("return JS_FALSE;\n");
                    printer->Outdent();
                    printer->Print("}\n");
                    printer->Print("\n");
                }
                
                printer->Print("JSBool JS$classname$::js_has_$fieldname$(JSContext *cx, uint32_t argc, jsval *vp)\n", "fieldname", fieldname, "classname", classname_);
                printer->Print("{\n");
                printer->Indent();
                printer->Print("JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);\n");
                printer->Print("JS$classname$* thiz = (JS$classname$*)JS_GetPrivate(obj);\n", "classname", classname_);
                printer->Print("JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(thiz->getNativeObject()->has_$fieldname$()));\n", "fieldname", fieldname);
                printer->Print("\n");
                printer->Print("return JS_TRUE;\n");
                printer->Outdent();
                printer->Print("}\n");
                printer->Print("\n");
            }
        }
    }

void MessageGenerator::
GenerateInlineMethods(io::Printer* printer) {
  for (int i = 0; i < descriptor_->nested_type_count(); i++) {
    nested_generators_[i]->GenerateInlineMethods(printer);
    printer->Print(kThinSeparator);
    printer->Print("\n");
  }

  GenerateFieldAccessorDefinitions(printer);
}

void MessageGenerator::
GenerateDescriptorDeclarations(io::Printer* printer) {
  printer->Print(
    "const ::google::protobuf::Descriptor* $name$_descriptor_ = NULL;\n"
    "const ::google::protobuf::internal::GeneratedMessageReflection*\n"
    "  $name$_reflection_ = NULL;\n",
    "name", classname_);

  for (int i = 0; i < descriptor_->nested_type_count(); i++) {
    nested_generators_[i]->GenerateDescriptorDeclarations(printer);
  }

  for (int i = 0; i < descriptor_->enum_type_count(); i++) {
    printer->Print(
      "const ::google::protobuf::EnumDescriptor* $name$_descriptor_ = NULL;\n",
      "name", ClassName(descriptor_->enum_type(i), false));
  }
}

void MessageGenerator::
GenerateDescriptorInitializer(io::Printer* printer, int index) {
  // TODO(kenton):  Passing the index to this method is redundant; just use
  //   descriptor_->index() instead.
  map<string, string> vars;
  vars["classname"] = classname_;
  vars["index"] = SimpleItoa(index);

  // Obtain the descriptor from the parent's descriptor.
  if (descriptor_->containing_type() == NULL) {
    printer->Print(vars,
      "$classname$_descriptor_ = file->message_type($index$);\n");
  } else {
    vars["parent"] = ClassName(descriptor_->containing_type(), false);
    printer->Print(vars,
      "$classname$_descriptor_ = "
        "$parent$_descriptor_->nested_type($index$);\n");
  }

  // Generate the offsets.
  GenerateOffsets(printer);

  // Construct the reflection object.
  printer->Print(vars,
    "$classname$_reflection_ =\n"
    "  new ::google::protobuf::internal::GeneratedMessageReflection(\n"
    "    $classname$_descriptor_,\n"
    "    $classname$::default_instance_,\n"
    "    $classname$_offsets_,\n"
    "    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET($classname$, _has_bits_[0]),\n"
    "    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET("
      "$classname$, _unknown_fields_),\n");
  if (descriptor_->extension_range_count() > 0) {
    printer->Print(vars,
      "    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET("
        "$classname$, _extensions_),\n");
  } else {
    // No extensions.
    printer->Print(vars,
      "    -1,\n");
  }
  printer->Print(
    "    ::google::protobuf::DescriptorPool::generated_pool(),\n");
  printer->Print(vars,
    "    ::google::protobuf::MessageFactory::generated_factory(),\n");
  printer->Print(vars,
    "    sizeof($classname$));\n");

  // Handle nested types.
  for (int i = 0; i < descriptor_->nested_type_count(); i++) {
    nested_generators_[i]->GenerateDescriptorInitializer(printer, i);
  }

  for (int i = 0; i < descriptor_->enum_type_count(); i++) {
    enum_generators_[i]->GenerateDescriptorInitializer(printer, i);
  }
}

void MessageGenerator::
GenerateTypeRegistrations(io::Printer* printer) {
  // Register this message type with the message factory.
  printer->Print(
    "::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(\n"
    "  $classname$_descriptor_, &$classname$::default_instance());\n",
    "classname", classname_);

  // Handle nested types.
  for (int i = 0; i < descriptor_->nested_type_count(); i++) {
    nested_generators_[i]->GenerateTypeRegistrations(printer);
  }
}

void MessageGenerator::
GenerateDefaultInstanceAllocator(io::Printer* printer) {
  // Construct the default instances of all fields, as they will be used
  // when creating the default instance of the entire message.
  for (int i = 0; i < descriptor_->field_count(); i++) {
    field_generators_.get(descriptor_->field(i))
                     .GenerateDefaultInstanceAllocator(printer);
  }

  // Construct the default instance.  We can't call InitAsDefaultInstance() yet
  // because we need to make sure all default instances that this one might
  // depend on are constructed first.
  printer->Print(
    "$classname$::default_instance_ = new $classname$();\n",
    "classname", classname_);

  // Handle nested types.
  for (int i = 0; i < descriptor_->nested_type_count(); i++) {
    nested_generators_[i]->GenerateDefaultInstanceAllocator(printer);
  }

}

void MessageGenerator::
GenerateDefaultInstanceInitializer(io::Printer* printer) {
  printer->Print(
    "$classname$::default_instance_->InitAsDefaultInstance();\n",
    "classname", classname_);

  // Register extensions.
  for (int i = 0; i < descriptor_->extension_count(); i++) {
    extension_generators_[i]->GenerateRegistration(printer);
  }

  // Handle nested types.
  for (int i = 0; i < descriptor_->nested_type_count(); i++) {
    nested_generators_[i]->GenerateDefaultInstanceInitializer(printer);
  }
}

void MessageGenerator::
GenerateShutdownCode(io::Printer* printer) {
  printer->Print(
    "delete $classname$::default_instance_;\n",
    "classname", classname_);

  if (HasDescriptorMethods(descriptor_->file())) {
    printer->Print(
      "delete $classname$_reflection_;\n",
      "classname", classname_);
  }

  // Handle default instances of fields.
  for (int i = 0; i < descriptor_->field_count(); i++) {
    field_generators_.get(descriptor_->field(i))
                     .GenerateShutdownCode(printer);
  }

  // Handle nested types.
  for (int i = 0; i < descriptor_->nested_type_count(); i++) {
    nested_generators_[i]->GenerateShutdownCode(printer);
  }
}

void MessageGenerator::
GenerateClassMethods(io::Printer* printer) {
  for (int i = 0; i < descriptor_->enum_type_count(); i++) {
    enum_generators_[i]->GenerateMethods(printer);
  }

  for (int i = 0; i < descriptor_->nested_type_count(); i++) {
    nested_generators_[i]->GenerateClassMethods(printer);
    printer->Print("\n");
    printer->Print(kThinSeparator);
    printer->Print("\n");
  }

  // Generate non-inline field definitions.
  for (int i = 0; i < descriptor_->field_count(); i++) {
    field_generators_.get(descriptor_->field(i))
                     .GenerateNonInlineAccessorDefinitions(printer);
  }

  // Generate field number constants.
  printer->Print("#ifndef _MSC_VER\n");
  for (int i = 0; i < descriptor_->field_count(); i++) {
    const FieldDescriptor *field = descriptor_->field(i);
    printer->Print(
      "const int $classname$::$constant_name$;\n",
      "classname", ClassName(FieldScope(field), false),
      "constant_name", FieldConstantName(field));
  }
  printer->Print(
    "#endif  // !_MSC_VER\n"
    "\n");

  // Define extension identifiers.
  for (int i = 0; i < descriptor_->extension_count(); i++) {
    extension_generators_[i]->GenerateDefinition(printer);
  }

  GenerateStructors(printer);
  printer->Print("\n");

  if (HasGeneratedMethods(descriptor_->file())) {
    GenerateClear(printer);
    printer->Print("\n");

    GenerateMergeFromCodedStream(printer);
    printer->Print("\n");

    GenerateSerializeWithCachedSizes(printer);
    printer->Print("\n");

    if (HasFastArraySerialization(descriptor_->file())) {
      GenerateSerializeWithCachedSizesToArray(printer);
      printer->Print("\n");
    }

    GenerateByteSize(printer);
    printer->Print("\n");

    GenerateMergeFrom(printer);
    printer->Print("\n");

    GenerateCopyFrom(printer);
    printer->Print("\n");
      
      if (jsbinding_) {
          printer->Print("#if USE_JS_SCRIPTING\n");
          GenerateCopyToJSObject(printer);
          printer->Print("\n");
          GenerateCopyFromJSObject(printer);
          printer->Print("\n");
          printer->Print("jsval $classname$::ToJSVAL() {\n", "classname", classname_);
          printer->Indent();
          printer->Print("return ::hoolai::value_to_jsval< $classname$* >(this);\n", "classname", classname_);
          printer->Outdent();
          printer->Print("}\n");
          printer->Print("\n");
          printer->Print("#endif\n");
      }

    GenerateIsInitialized(printer);
    printer->Print("\n");
  }

  GenerateSwap(printer);
  printer->Print("\n");

  if (HasDescriptorMethods(descriptor_->file())) {
    printer->Print(
      "::google::protobuf::Metadata $classname$::GetMetadata() const {\n"
      "  protobuf_AssignDescriptorsOnce();\n"
      "  ::google::protobuf::Metadata metadata;\n"
      "  metadata.descriptor = $classname$_descriptor_;\n"
      "  metadata.reflection = $classname$_reflection_;\n"
      "  return metadata;\n"
      "}\n"
      "\n",
      "classname", classname_);
  } else {
    printer->Print(
      "::std::string $classname$::GetTypeName() const {\n"
      "  return \"$type_name$\";\n"
      "}\n"
      "\n",
      "classname", classname_,
      "type_name", descriptor_->full_name());
  }
    
    GeneratePrintDebugInfoFunction(printer);

}

void MessageGenerator::
GenerateOffsets(io::Printer* printer) {
  printer->Print(
    "static const int $classname$_offsets_[$field_count$] = {\n",
    "classname", classname_,
    "field_count", SimpleItoa(max(1, descriptor_->field_count())));
  printer->Indent();

  for (int i = 0; i < descriptor_->field_count(); i++) {
    const FieldDescriptor* field = descriptor_->field(i);
    printer->Print(
      "GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET($classname$, $name$_),\n",
      "classname", classname_,
      "name", FieldName(field));
  }

  printer->Outdent();
  printer->Print("};\n");
}

void MessageGenerator::
GenerateSharedConstructorCode(io::Printer* printer) {
  printer->Print(
    "void $classname$::SharedCtor() {\n",
    "classname", classname_);
  printer->Indent();

  printer->Print(
    "_cached_size_ = 0;\n");

  for (int i = 0; i < descriptor_->field_count(); i++) {
    field_generators_.get(descriptor_->field(i))
                     .GenerateConstructorCode(printer);
  }

  printer->Print(
    "::memset(_has_bits_, 0, sizeof(_has_bits_));\n");

  printer->Outdent();
  printer->Print("}\n\n");
}

void MessageGenerator::
GenerateSharedDestructorCode(io::Printer* printer) {
  printer->Print(
    "void $classname$::SharedDtor() {\n",
    "classname", classname_);
  printer->Indent();
  // Write the destructors for each field.
  for (int i = 0; i < descriptor_->field_count(); i++) {
    field_generators_.get(descriptor_->field(i))
                     .GenerateDestructorCode(printer);
  }

  PrintHandlingOptionalStaticInitializers(
    descriptor_->file(), printer,
    // With static initializers.
    "if (this != default_instance_) {\n",
    // Without.
    "if (this != &default_instance()) {\n");

  // We need to delete all embedded messages.
  // TODO(kenton):  If we make unset messages point at default instances
  //   instead of NULL, then it would make sense to move this code into
  //   MessageFieldGenerator::GenerateDestructorCode().
  for (int i = 0; i < descriptor_->field_count(); i++) {
    const FieldDescriptor* field = descriptor_->field(i);

    if (!field->is_repeated() &&
        field->cpp_type() == FieldDescriptor::CPPTYPE_MESSAGE) {
      printer->Print("  delete $name$_;\n",
                     "name", FieldName(field));
    }
  }

  printer->Outdent();
  printer->Print(
    "  }\n"
    "}\n"
    "\n");
}

void MessageGenerator::
GenerateStructors(io::Printer* printer) {
  string superclass = SuperClassName(descriptor_);

  // Generate the default constructor.
  printer->Print(
    "$classname$::$classname$()\n"
    "  : $superclass$() {\n"
    "  SharedCtor();\n"
    "}\n",
    "classname", classname_,
    "superclass", superclass);

  printer->Print(
    "\n"
    "void $classname$::InitAsDefaultInstance() {\n",
    "classname", classname_);

  // The default instance needs all of its embedded message pointers
  // cross-linked to other default instances.  We can't do this initialization
  // in the constructor because some other default instances may not have been
  // constructed yet at that time.
  // TODO(kenton):  Maybe all message fields (even for non-default messages)
  //   should be initialized to point at default instances rather than NULL?
  for (int i = 0; i < descriptor_->field_count(); i++) {
    const FieldDescriptor* field = descriptor_->field(i);

    if (!field->is_repeated() &&
        field->cpp_type() == FieldDescriptor::CPPTYPE_MESSAGE) {
      PrintHandlingOptionalStaticInitializers(
        descriptor_->file(), printer,
        // With static initializers.
        "  $name$_ = const_cast< $type$*>(&$type$::default_instance());\n",
        // Without.
        "  $name$_ = const_cast< $type$*>(\n"
        "      $type$::internal_default_instance());\n",
        // Vars.
        "name", FieldName(field),
        "type", FieldMessageTypeName(field));
    }
  }
  printer->Print(
    "}\n"
    "\n");

  // Generate the copy constructor.
  printer->Print(
    "$classname$::$classname$(const $classname$& from)\n"
    "  : $superclass$() {\n"
    "  SharedCtor();\n"
    "  MergeFrom(from);\n"
    "}\n"
    "\n",
    "classname", classname_,
    "superclass", superclass);

  // Generate the shared constructor code.
  GenerateSharedConstructorCode(printer);

  // Generate the destructor.
    if (jsbinding_) {
        printer->Print(
                       "$classname$::~$classname$() {\n"
                       "  SharedDtor();\n#if USE_JS_SCRIPTING\n  JS$classname$::removeJSObject(this);\n#endif\n"
                       "}\n"
                       "\n",
                       "classname", classname_);
    } else {
      printer->Print(
        "$classname$::~$classname$() {\n"
        "  SharedDtor();\n"
        "}\n"
        "\n",
        "classname", classname_);
    }

  // Generate the shared destructor code.
  GenerateSharedDestructorCode(printer);

  // Generate SetCachedSize.
  printer->Print(
    "void $classname$::SetCachedSize(int size) const {\n"
    "  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();\n"
    "  _cached_size_ = size;\n"
    "  GOOGLE_SAFE_CONCURRENT_WRITES_END();\n"
    "}\n",
    "classname", classname_);

  // Only generate this member if it's not disabled.
  if (HasDescriptorMethods(descriptor_->file()) &&
      !descriptor_->options().no_standard_descriptor_accessor()) {
    printer->Print(
      "const ::google::protobuf::Descriptor* $classname$::descriptor() {\n"
      "  protobuf_AssignDescriptorsOnce();\n"
      "  return $classname$_descriptor_;\n"
      "}\n"
      "\n",
      "classname", classname_,
      "adddescriptorsname",
      GlobalAddDescriptorsName(descriptor_->file()->name()));
  }

  printer->Print(
    "const $classname$& $classname$::default_instance() {\n",
    "classname", classname_);

  PrintHandlingOptionalStaticInitializers(
    descriptor_->file(), printer,
    // With static initializers.
    "  if (default_instance_ == NULL) $adddescriptorsname$();\n",
    // Without.
    "  $adddescriptorsname$();\n",
    // Vars.
    "adddescriptorsname",
    GlobalAddDescriptorsName(descriptor_->file()->name()));

  printer->Print(
    "  return *default_instance_;\n"
    "}\n"
    "\n"
    "$classname$* $classname$::default_instance_ = NULL;\n"
    "\n"
    "$classname$* $classname$::New() const {\n"
    "  return new $classname$;\n"
    "}\n",
    "classname", classname_,
    "adddescriptorsname",
    GlobalAddDescriptorsName(descriptor_->file()->name()));
}

void MessageGenerator::
GenerateClear(io::Printer* printer) {
  printer->Print("void $classname$::Clear() {\n",
                 "classname", classname_);
  printer->Indent();

  int last_index = -1;

  if (descriptor_->extension_range_count() > 0) {
    printer->Print("_extensions_.Clear();\n");
  }

  for (int i = 0; i < descriptor_->field_count(); i++) {
    const FieldDescriptor* field = descriptor_->field(i);

    if (!field->is_repeated()) {
      // We can use the fact that _has_bits_ is a giant bitfield to our
      // advantage:  We can check up to 32 bits at a time for equality to
      // zero, and skip the whole range if so.  This can improve the speed
      // of Clear() for messages which contain a very large number of
      // optional fields of which only a few are used at a time.  Here,
      // we've chosen to check 8 bits at a time rather than 32.
      if (i / 8 != last_index / 8 || last_index < 0) {
        if (last_index >= 0) {
          printer->Outdent();
          printer->Print("}\n");
        }
        printer->Print(
          "if (_has_bits_[$index$ / 32] & (0xffu << ($index$ % 32))) {\n",
          "index", SimpleItoa(field->index()));
        printer->Indent();
      }
      last_index = i;

      // It's faster to just overwrite primitive types, but we should
      // only clear strings and messages if they were set.
      // TODO(kenton):  Let the CppFieldGenerator decide this somehow.
      bool should_check_bit =
        field->cpp_type() == FieldDescriptor::CPPTYPE_MESSAGE ||
        field->cpp_type() == FieldDescriptor::CPPTYPE_STRING;

      if (should_check_bit) {
        printer->Print(
          "if (has_$name$()) {\n",
          "name", FieldName(field));
        printer->Indent();
      }

      field_generators_.get(field).GenerateClearingCode(printer);

      if (should_check_bit) {
        printer->Outdent();
        printer->Print("}\n");
      }
    }
  }

  if (last_index >= 0) {
    printer->Outdent();
    printer->Print("}\n");
  }

  // Repeated fields don't use _has_bits_ so we clear them in a separate
  // pass.
  for (int i = 0; i < descriptor_->field_count(); i++) {
    const FieldDescriptor* field = descriptor_->field(i);

    if (field->is_repeated()) {
      field_generators_.get(field).GenerateClearingCode(printer);
    }
  }

  printer->Print(
    "::memset(_has_bits_, 0, sizeof(_has_bits_));\n");

  if (HasUnknownFields(descriptor_->file())) {
    printer->Print(
      "mutable_unknown_fields()->Clear();\n");
  }

  printer->Outdent();
  printer->Print("}\n");
}

void MessageGenerator::
GenerateSwap(io::Printer* printer) {
  // Generate the Swap member function.
  printer->Print("void $classname$::Swap($classname$* other) {\n",
                 "classname", classname_);
  printer->Indent();
  printer->Print("if (other != this) {\n");
  printer->Indent();

  if (HasGeneratedMethods(descriptor_->file())) {
    for (int i = 0; i < descriptor_->field_count(); i++) {
      const FieldDescriptor* field = descriptor_->field(i);
      field_generators_.get(field).GenerateSwappingCode(printer);
    }

    for (int i = 0; i < (descriptor_->field_count() + 31) / 32; ++i) {
      printer->Print("std::swap(_has_bits_[$i$], other->_has_bits_[$i$]);\n",
                     "i", SimpleItoa(i));
    }

    if (HasUnknownFields(descriptor_->file())) {
      printer->Print("_unknown_fields_.Swap(&other->_unknown_fields_);\n");
    }
    printer->Print("std::swap(_cached_size_, other->_cached_size_);\n");
    if (descriptor_->extension_range_count() > 0) {
      printer->Print("_extensions_.Swap(&other->_extensions_);\n");
    }
  } else {
    printer->Print("GetReflection()->Swap(this, other);");
  }

  printer->Outdent();
  printer->Print("}\n");
  printer->Outdent();
  printer->Print("}\n");
}

void MessageGenerator::
GenerateMergeFrom(io::Printer* printer) {
  if (HasDescriptorMethods(descriptor_->file())) {
    // Generate the generalized MergeFrom (aka that which takes in the Message
    // base class as a parameter).
    printer->Print(
      "void $classname$::MergeFrom(const ::google::protobuf::Message& from) {\n"
      "  GOOGLE_CHECK_NE(&from, this);\n",
      "classname", classname_);
    printer->Indent();

    // Cast the message to the proper type. If we find that the message is
    // *not* of the proper type, we can still call Merge via the reflection
    // system, as the GOOGLE_CHECK above ensured that we have the same descriptor
    // for each message.
    printer->Print(
      "const $classname$* source =\n"
      "  ::google::protobuf::internal::dynamic_cast_if_available<const $classname$*>(\n"
      "    &from);\n"
      "if (source == NULL) {\n"
      "  ::google::protobuf::internal::ReflectionOps::Merge(from, this);\n"
      "} else {\n"
      "  MergeFrom(*source);\n"
      "}\n",
      "classname", classname_);

    printer->Outdent();
    printer->Print("}\n\n");
  } else {
    // Generate CheckTypeAndMergeFrom().
    printer->Print(
      "void $classname$::CheckTypeAndMergeFrom(\n"
      "    const ::google::protobuf::MessageLite& from) {\n"
      "  MergeFrom(*::google::protobuf::down_cast<const $classname$*>(&from));\n"
      "}\n"
      "\n",
      "classname", classname_);
  }

  // Generate the class-specific MergeFrom, which avoids the GOOGLE_CHECK and cast.
  printer->Print(
    "void $classname$::MergeFrom(const $classname$& from) {\n"
    "  GOOGLE_CHECK_NE(&from, this);\n",
    "classname", classname_);
  printer->Indent();

  // Merge Repeated fields. These fields do not require a
  // check as we can simply iterate over them.
  for (int i = 0; i < descriptor_->field_count(); ++i) {
    const FieldDescriptor* field = descriptor_->field(i);

    if (field->is_repeated()) {
      field_generators_.get(field).GenerateMergingCode(printer);
    }
  }

  // Merge Optional and Required fields (after a _has_bit check).
  int last_index = -1;

  for (int i = 0; i < descriptor_->field_count(); ++i) {
    const FieldDescriptor* field = descriptor_->field(i);

    if (!field->is_repeated()) {
      // See above in GenerateClear for an explanation of this.
      if (i / 8 != last_index / 8 || last_index < 0) {
        if (last_index >= 0) {
          printer->Outdent();
          printer->Print("}\n");
        }
        printer->Print(
          "if (from._has_bits_[$index$ / 32] & (0xffu << ($index$ % 32))) {\n",
          "index", SimpleItoa(field->index()));
        printer->Indent();
      }

      last_index = i;

      printer->Print(
        "if (from.has_$name$()) {\n",
        "name", FieldName(field));
      printer->Indent();

      field_generators_.get(field).GenerateMergingCode(printer);

      printer->Outdent();
      printer->Print("}\n");
    }
  }

  if (last_index >= 0) {
    printer->Outdent();
    printer->Print("}\n");
  }

  if (descriptor_->extension_range_count() > 0) {
    printer->Print("_extensions_.MergeFrom(from._extensions_);\n");
  }

  if (HasUnknownFields(descriptor_->file())) {
    printer->Print(
      "mutable_unknown_fields()->MergeFrom(from.unknown_fields());\n");
  }

  printer->Outdent();
  printer->Print("}\n");
}

void MessageGenerator::
GenerateCopyFrom(io::Printer* printer) {
  if (HasDescriptorMethods(descriptor_->file())) {
    // Generate the generalized CopyFrom (aka that which takes in the Message
    // base class as a parameter).
    printer->Print(
      "void $classname$::CopyFrom(const ::google::protobuf::Message& from) {\n",
      "classname", classname_);
    printer->Indent();

    printer->Print(
      "if (&from == this) return;\n"
      "Clear();\n"
      "MergeFrom(from);\n");

    printer->Outdent();
    printer->Print("}\n\n");
  }

  // Generate the class-specific CopyFrom.
  printer->Print(
    "void $classname$::CopyFrom(const $classname$& from) {\n",
    "classname", classname_);
  printer->Indent();

  printer->Print(
    "if (&from == this) return;\n"
    "Clear();\n"
    "MergeFrom(from);\n");

  printer->Outdent();
  printer->Print("}\n");
}
    
    void MessageGenerator::
    GenerateCopyFromJSObject(io::Printer* printer)
    {
        printer->Print(
                       "void $classname$::CopyFromJSObject(JSObject* jsobj) {\n",
                       "classname", classname_);
        printer->Indent();
        printer->Print("Clear();\n");
        printer->Print("JSBool found;\n");
        printer->Print("JSContext * cx = ::hoolai::JSScriptingCore::getSingleton()->getGlobalContext();\n");
        printer->Print("JS::RootedValue prop(cx);\n");
        for (int i = 0; i < descriptor_->field_count(); i++) {
            const FieldDescriptor* field = descriptor_->field(i);
            string fieldname = FieldName(field);
            printer->Print("JS_HasProperty(cx, jsobj, \"$fieldname$\", &found);\n", "fieldname", fieldname);
            printer->Print("if (found) {\n");
            printer->Indent();
            printer->Print("JS_GetProperty(cx, jsobj, \"$fieldname$\", &prop);\n", "fieldname", fieldname);
            if (field->is_repeated()) {
                printer->Print("JSObject * arrayObj = prop.toObjectOrNull();\n");
                printer->Print("uint32_t length;\n");
                printer->Print("JS_GetArrayLength(cx, arrayObj, &length);\n");
                printer->Print("for (int i = 0; i < length; ++i) {\n");
                printer->Indent();
                printer->Print("JS::RootedValue val(cx);\n");
                printer->Print("JS_GetElement(cx, arrayObj, i, &val);\n");
                if (field->type() == FieldDescriptor::TYPE_MESSAGE) {
                    printer->Print("$classname$ *add = add_$fieldname$();\n", "fieldname", fieldname, "classname", FieldMessageTypeName(field));
                    printer->Print("add->CopyFromJSObject(JSVAL_TO_OBJECT(val));\n");
                } else if (field->type() == FieldDescriptor::TYPE_STRING) {
                    printer->Print("JSString* value = JSVAL_TO_STRING(val);\n");
                    printer->Print("char* s = JS_EncodeStringToUTF8(cx, value);\n");
                    printer->Print("add_$fieldname$(s);\n", "fieldname", fieldname);
                    printer->Print("JS_free(cx, s);\n");
                } else if (field->type() == FieldDescriptor::TYPE_BOOL) {
                    printer->Print("bool value = JSVAL_TO_BOOLEAN(val);\n");
                    printer->Print("add_$fieldname$(value);\n", "fieldname", fieldname);
                } else if (field->type() == FieldDescriptor::TYPE_INT32 ||
                           field->type() == FieldDescriptor::TYPE_FIXED32 ||
                           field->type() == FieldDescriptor::TYPE_SFIXED32 ||
                           field->type() == FieldDescriptor::TYPE_SINT32 ||
                           field->type() == FieldDescriptor::TYPE_ENUM) {
                    printer->Print("int32_t value = JSVAL_TO_INT(val);\n");
                    printer->Print("add_$fieldname$(value);\n", "fieldname", fieldname);
                } else if (field->type() == FieldDescriptor::TYPE_UINT32) {
                    printer->Print("uint32_t value = JSVAL_TO_UINT(val);\n");
                    printer->Print("add_$fieldname$(value);\n", "fieldname", fieldname);
                } else if (field->type() == FieldDescriptor::TYPE_DOUBLE) {
                    printer->Print("double value = JSVAL_IS_INT(val)?double(JSVAL_TO_INT(val)):JSVAL_TO_DOUBLE(val);\n");
                    printer->Print("add_$fieldname$(value);\n", "fieldname", fieldname);
                } else if (field->type() == FieldDescriptor::TYPE_FLOAT) {
                    printer->Print("double value = JSVAL_IS_INT(val)?double(JSVAL_TO_INT(val)):JSVAL_TO_DOUBLE(val);\n");
                    printer->Print("add_$fieldname$(value);\n", "fieldname", fieldname);
                } else if (field->type() == FieldDescriptor::TYPE_INT64) {
                    printer->Print("double value = JSVAL_IS_INT(val)?double(JSVAL_TO_INT(val)):JSVAL_TO_DOUBLE(val);\n");
                    printer->Print("add_$fieldname$((int64_t)value);\n", "fieldname", fieldname);
                } else if (field->type() == FieldDescriptor::TYPE_UINT64) {
                    printer->Print("double value = JSVAL_IS_INT(val)?double(JSVAL_TO_INT(val)):JSVAL_TO_DOUBLE(val);\n");
                    printer->Print("add_$fieldname$((uint64_t)value);\n", "fieldname", fieldname);
                } else if (field->type() == FieldDescriptor::TYPE_BYTES) {
                    printer->Print("JSObject *arg0 = val.toObjectOrNull();\n");
                    printer->Print("uint32_t len = JS_GetArrayBufferByteLength(arg0);\n");
                    printer->Print("uint8_t *data = JS_GetArrayBufferData(arg0);\n");
                    printer->Print("std::string s((const char*)data, len);\n");
                    printer->Print("add_$fieldname$(s);\n", "fieldname", fieldname);
                }
                printer->Outdent();
                printer->Print("}\n");
            } else {
                if (field->type() == FieldDescriptor::TYPE_MESSAGE) {
                    printer->Print("JSObject * obj = JSVAL_TO_OBJECT(prop);\n");
                    printer->Print("mutable_$fieldname$()->CopyFromJSObject(obj);\n", "fieldname", fieldname);
                } else if (field->type() == FieldDescriptor::TYPE_STRING) {
                    printer->Print("JSString* value = JSVAL_TO_STRING(prop);\n");
                    printer->Print("char* s = JS_EncodeStringToUTF8(cx, value);\n");
                    printer->Print("set_$fieldname$(s);\n", "fieldname", fieldname);
                    printer->Print("JS_free(cx, s);\n");
                } else if (field->type() == FieldDescriptor::TYPE_BOOL) {
                    printer->Print("JSBool value = JSVAL_TO_BOOLEAN(prop);\n");
                    printer->Print("set_$fieldname$(value);\n", "fieldname", fieldname);
                } else if (field->type() == FieldDescriptor::TYPE_INT32 ||
                           field->type() == FieldDescriptor::TYPE_FIXED32 ||
                           field->type() == FieldDescriptor::TYPE_SFIXED32 ||
                           field->type() == FieldDescriptor::TYPE_SINT32 ||
                           field->type() == FieldDescriptor::TYPE_ENUM) {
                    printer->Print("int32_t value = JSVAL_TO_INT(prop);\n");
                    printer->Print("set_$fieldname$(value);\n", "fieldname", fieldname);
                } else if (field->type() == FieldDescriptor::TYPE_UINT32) {
                    printer->Print("uint32_t value = JSVAL_TO_UINT(prop);\n");
                    printer->Print("set_$fieldname$(value);\n", "fieldname", fieldname);
                } else if (field->type() == FieldDescriptor::TYPE_DOUBLE) {
                    printer->Print("double value = JSVAL_IS_INT(prop)?double(JSVAL_TO_INT(prop)):JSVAL_TO_DOUBLE(prop);\n");
                    printer->Print("set_$fieldname$(value);\n", "fieldname", fieldname);
                } else if (field->type() == FieldDescriptor::TYPE_FLOAT) {
                    printer->Print("double value = JSVAL_IS_INT(prop)?double(JSVAL_TO_INT(prop)):JSVAL_TO_DOUBLE(prop);\n");
                    printer->Print("set_$fieldname$(value);\n", "fieldname", fieldname);
                } else if (field->type() == FieldDescriptor::TYPE_INT64) {
                    printer->Print("double value = JSVAL_IS_INT(prop)?double(JSVAL_TO_INT(prop)):JSVAL_TO_DOUBLE(prop);\n");
                    printer->Print("set_$fieldname$((int64_t)value);\n", "fieldname", fieldname);
                } else if (field->type() == FieldDescriptor::TYPE_UINT64) {
                    printer->Print("double value = JSVAL_IS_INT(prop)?double(JSVAL_TO_INT(prop)):JSVAL_TO_DOUBLE(prop);\n");
                    printer->Print("set_$fieldname$((uint64_t)value);\n", "fieldname", fieldname);
                } else if (field->type() == FieldDescriptor::TYPE_BYTES) {
                    printer->Print("JSObject *arg0 = prop.toObjectOrNull();\n");
                    printer->Print("uint32_t len = JS_GetArrayBufferByteLength(arg0);\n");
                    printer->Print("uint8_t *data = JS_GetArrayBufferData(arg0);\n");
                    printer->Print("std::string s((const char*)data, len);\n");
                    printer->Print("set_$fieldname$(s);\n", "fieldname", fieldname);
                }
            }
            printer->Outdent();
            printer->Print("}\n");
        }
        printer->Outdent();
        printer->Print("}\n");
    }
    
    void MessageGenerator::
    GenerateCopyToJSObject(io::Printer* printer)
    {                                                //JS_SetProperty(cx, m_jsobject, name, &val);
        printer->Print("void $classname$::CopyToJSObject(JSObject* jsobj) {\n",
                       "classname", classname_);
        printer->Indent();
        printer->Print("JSContext *cx = ::hoolai::JSScriptingCore::getSingleton()->getGlobalContext();\n");
        printer->Print("JSObject *o;\n");
        printer->Print("JS::RootedValue val(cx);\n");
        printer->Print("JSString* jstr;\n");
        for (int i = 0; i < descriptor_->field_count(); i++) {
            const FieldDescriptor* field = descriptor_->field(i);
            string fieldname = FieldName(field);
            if (field->is_repeated()) {
                printer->Print("{\n");
                printer->Print("int size = (int)$fieldname$_size();\n", "fieldname", fieldname);
                printer->Print("jsval *valarr = new jsval[size];\n");
                printer->Print("for (int i = 0; i < size; ++i) {\n");
                printer->Indent();
                if (field->type() == FieldDescriptor::TYPE_MESSAGE) {
                    printer->Print("o = JS_NewObject(cx, NULL, NULL, NULL);\n");
                    printer->Print("mutable_$fieldname$(i)->CopyToJSObject(o);\n", "fieldname", fieldname);
                    printer->Print("valarr[i] = OBJECT_TO_JSVAL(o);\n");
                } else if (field->type() == FieldDescriptor::TYPE_STRING) {
                    printer->Print("valarr[i] = ::hoolai::value_to_jsval($fieldname$(i).c_str());\n", "fieldname", fieldname);
                } else if (field->type() == FieldDescriptor::TYPE_BOOL) {
                    printer->Print("valarr[i] = BOOLEAN_TO_JSVAL((JSBool)$fieldname$(i));\n", "fieldname", fieldname);
                } else if (field->type() == FieldDescriptor::TYPE_INT32 ||
                           field->type() == FieldDescriptor::TYPE_FIXED32 ||
                           field->type() == FieldDescriptor::TYPE_SFIXED32 ||
                           field->type() == FieldDescriptor::TYPE_SINT32 ||
                           field->type() == FieldDescriptor::TYPE_ENUM) {
                    printer->Print("valarr[i] = INT_TO_JSVAL($fieldname$(i));\n", "fieldname", fieldname);
                } else if (field->type() == FieldDescriptor::TYPE_UINT32) {
                    printer->Print("valarr[i] = UINT_TO_JSVAL($fieldname$(i));\n", "fieldname", fieldname);
                } else if (field->type() == FieldDescriptor::TYPE_DOUBLE) {
                    printer->Print("valarr[i] = DOUBLE_TO_JSVAL($fieldname$(i));\n", "fieldname", fieldname);
                } else if (field->type() == FieldDescriptor::TYPE_FLOAT) {
                    printer->Print("valarr[i] = DOUBLE_TO_JSVAL($fieldname$(i));\n", "fieldname", fieldname);
                } else if (field->type() == FieldDescriptor::TYPE_INT64 ||
                           field->type() == FieldDescriptor::TYPE_UINT64) {
                    printer->Print("valarr[i] = DOUBLE_TO_JSVAL($fieldname$(i));\n", "fieldname", fieldname);
                } else if (field->type() == FieldDescriptor::TYPE_BYTES) {
                    printer->Print("void * contents = NULL;\n");
                    printer->Print("uint8_t * data = NULL;\n");
                    printer->Print("const std::string &value = $fieldname$(i);\n", "fieldname", fieldname);
                    printer->Print("if (JS_AllocateArrayBufferContents(cx, (uint32_t)value.length(), &contents, &data)) {\n");
                    printer->Indent();
                    printer->Print("memcpy(data, value.c_str(), value.length());\n");
                    printer->Print("valarr[i] = OBJECT_TO_JSVAL(JS_NewArrayBufferWithContents(cx, contents));\n");
                    printer->Outdent();
                    printer->Print("} else {valarr[i] = JSVAL_NULL;}\n");
                }
                printer->Outdent();
                printer->Print("}\n");
                printer->Print("JSObject *array = JS_NewArrayObject(cx, size, valarr);\n");
                printer->Print("val.setObjectOrNull(array);\n");
                printer->Print("JS_SetProperty(cx, jsobj, \"$fieldname$\", val);\n", "fieldname", fieldname);
                printer->Print("delete[] valarr;\n");
                printer->Print("}\n");
            } else {
                printer->Print("if (has_$fieldname$()) {\n", "fieldname", fieldname);
                printer->Indent();
                if (field->type() == FieldDescriptor::TYPE_MESSAGE) {
                    printer->Print("o = JS_NewObject(cx, NULL, NULL, NULL);\n");
                    //                printer->Print("if(has_$fieldname$()) {\n", "fieldname", fieldname);
                    //                printer->Indent();
                    printer->Print("mutable_$fieldname$()->CopyToJSObject(o);\n", "fieldname", fieldname);
                    printer->Print("val.setObjectOrNull(o);\n");
                    printer->Print("JS_SetProperty(cx, jsobj, \"$fieldname$\", val);\n", "fieldname", fieldname);
                    //                printer->Outdent();
                    //                printer->Print("} else {\n");
                    //                printer->Indent();
                    //                printer->Print("val = JSVAL_NULL;\n");
                    //                printer->Print("JS_SetProperty(cx, jsobj, \"$fieldname$\", &val);\n", "fieldname", fieldname);
                    //                printer->Outdent();
                    //                printer->Print("}\n");
                } else if (field->type() == FieldDescriptor::TYPE_STRING) {
                    printer->Print("val.set(::hoolai::value_to_jsval($fieldname$().c_str()));\n", "fieldname", fieldname);
                    printer->Print("JS_SetProperty(cx, jsobj, \"$fieldname$\", val);\n", "fieldname", fieldname);
                } else if (field->type() == FieldDescriptor::TYPE_BOOL) {
                    printer->Print("val.setBoolean((JSBool)$fieldname$());\n", "fieldname", fieldname);
                    printer->Print("JS_SetProperty(cx, jsobj, \"$fieldname$\", val);\n", "fieldname", fieldname);
                } else if (field->type() == FieldDescriptor::TYPE_INT32 ||
                           field->type() == FieldDescriptor::TYPE_FIXED32 ||
                           field->type() == FieldDescriptor::TYPE_SFIXED32 ||
                           field->type() == FieldDescriptor::TYPE_SINT32 ||
                           field->type() == FieldDescriptor::TYPE_ENUM) {
                    printer->Print("val.setInt32($fieldname$());\n", "fieldname", fieldname);
                    printer->Print("JS_SetProperty(cx, jsobj, \"$fieldname$\", val);\n", "fieldname", fieldname);
                } else if (field->type() == FieldDescriptor::TYPE_UINT32) {
                    printer->Print("val.setInt32($fieldname$());\n", "fieldname", fieldname);
                    printer->Print("JS_SetProperty(cx, jsobj, \"$fieldname$\", val);\n", "fieldname", fieldname);
                } else if (field->type() == FieldDescriptor::TYPE_DOUBLE) {
                    printer->Print("val.setDouble($fieldname$());\n", "fieldname", fieldname);
                    printer->Print("JS_SetProperty(cx, jsobj, \"$fieldname$\", val);\n", "fieldname", fieldname);
                } else if (field->type() == FieldDescriptor::TYPE_FLOAT) {
                    printer->Print("val.setDouble($fieldname$());\n", "fieldname", fieldname);
                    printer->Print("JS_SetProperty(cx, jsobj, \"$fieldname$\", val);\n", "fieldname", fieldname);
                } else if (field->type() == FieldDescriptor::TYPE_INT64 ||
                           field->type() == FieldDescriptor::TYPE_UINT64) {
                    printer->Print("val.setDouble($fieldname$());\n", "fieldname", fieldname);
                    printer->Print("JS_SetProperty(cx, jsobj, \"$fieldname$\", val);\n", "fieldname", fieldname);
                } else if (field->type() == FieldDescriptor::TYPE_BYTES) {
                    printer->Print("void * contents = NULL;\n");
                    printer->Print("uint8_t * data = NULL;\n");
                    printer->Print("const std::string &value = $fieldname$();\n", "fieldname", fieldname);
                    printer->Print("if (JS_AllocateArrayBufferContents(cx, (uint32_t)value.length(), &contents, &data)) {\n");
                    printer->Indent();
                    printer->Print("memcpy(data, value.c_str(), value.length());\n");
                    printer->Print("val.setObjectOrNull(JS_NewArrayBufferWithContents(cx, contents));\n");
                    printer->Outdent();
                    printer->Print("} else {val.setNull();}\n");
                    printer->Print("JS_SetProperty(cx, jsobj, \"$fieldname$\", val);\n", "fieldname", fieldname);
                }
                printer->Outdent();
                printer->Print("}\n");
            }
        }
        printer->Outdent();
        printer->Print("}\n");
    }

void MessageGenerator::
GenerateMergeFromCodedStream(io::Printer* printer) {
  if (descriptor_->options().message_set_wire_format()) {
    // Special-case MessageSet.
    printer->Print(
      "bool $classname$::MergePartialFromCodedStream(\n"
      "    ::google::protobuf::io::CodedInputStream* input) {\n",
      "classname", classname_);

    PrintHandlingOptionalStaticInitializers(
      descriptor_->file(), printer,
      // With static initializers.
      "  return _extensions_.ParseMessageSet(input, default_instance_,\n"
      "                                      mutable_unknown_fields());\n",
      // Without.
      "  return _extensions_.ParseMessageSet(input, &default_instance(),\n"
      "                                      mutable_unknown_fields());\n",
      // Vars.
      "classname", classname_);

    printer->Print(
      "}\n");
    return;
  }

  printer->Print(
    "bool $classname$::MergePartialFromCodedStream(\n"
    "    ::google::protobuf::io::CodedInputStream* input) {\n"
    "#define DO_(EXPRESSION) if (!(EXPRESSION)) return false\n"
    "  ::google::protobuf::uint32 tag;\n"
    "  while ((tag = input->ReadTag()) != 0) {\n",
    "classname", classname_);

  printer->Indent();
  printer->Indent();

  if (descriptor_->field_count() > 0) {
    // We don't even want to print the switch() if we have no fields because
    // MSVC dislikes switch() statements that contain only a default value.

    // Note:  If we just switched on the tag rather than the field number, we
    // could avoid the need for the if() to check the wire type at the beginning
    // of each case.  However, this is actually a bit slower in practice as it
    // creates a jump table that is 8x larger and sparser, and meanwhile the
    // if()s are highly predictable.
    printer->Print(
      "switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {\n");

    printer->Indent();

    scoped_array<const FieldDescriptor*> ordered_fields(
      SortFieldsByNumber(descriptor_));

    for (int i = 0; i < descriptor_->field_count(); i++) {
      const FieldDescriptor* field = ordered_fields[i];

      PrintFieldComment(printer, field);

      printer->Print(
        "case $number$: {\n",
        "number", SimpleItoa(field->number()));
      printer->Indent();
      const FieldGenerator& field_generator = field_generators_.get(field);

      // Emit code to parse the common, expected case.
      printer->Print(
        "if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==\n"
        "    ::google::protobuf::internal::WireFormatLite::WIRETYPE_$wiretype$) {\n",
        "wiretype", kWireTypeNames[WireFormat::WireTypeForField(field)]);

      if (i > 0 || (field->is_repeated() && !field->options().packed())) {
        printer->Print(
          " parse_$name$:\n",
          "name", field->name());
      }

      printer->Indent();
      if (field->options().packed()) {
        field_generator.GenerateMergeFromCodedStreamWithPacking(printer);
      } else {
        field_generator.GenerateMergeFromCodedStream(printer);
      }
      printer->Outdent();

      // Emit code to parse unexpectedly packed or unpacked values.
      if (field->is_packable() && field->options().packed()) {
        printer->Print(
          "} else if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag)\n"
          "           == ::google::protobuf::internal::WireFormatLite::\n"
          "              WIRETYPE_$wiretype$) {\n",
          "wiretype",
          kWireTypeNames[WireFormat::WireTypeForFieldType(field->type())]);
        printer->Indent();
        field_generator.GenerateMergeFromCodedStream(printer);
        printer->Outdent();
      } else if (field->is_packable() && !field->options().packed()) {
        printer->Print(
          "} else if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag)\n"
          "           == ::google::protobuf::internal::WireFormatLite::\n"
          "              WIRETYPE_LENGTH_DELIMITED) {\n");
        printer->Indent();
        field_generator.GenerateMergeFromCodedStreamWithPacking(printer);
        printer->Outdent();
      }

      printer->Print(
        "} else {\n"
        "  goto handle_uninterpreted;\n"
        "}\n");

      // switch() is slow since it can't be predicted well.  Insert some if()s
      // here that attempt to predict the next tag.
      if (field->is_repeated() && !field->options().packed()) {
        // Expect repeats of this field.
        printer->Print(
          "if (input->ExpectTag($tag$)) goto parse_$name$;\n",
          "tag", SimpleItoa(WireFormat::MakeTag(field)),
          "name", field->name());
      }

      if (i + 1 < descriptor_->field_count()) {
        // Expect the next field in order.
        const FieldDescriptor* next_field = ordered_fields[i + 1];
        printer->Print(
          "if (input->ExpectTag($next_tag$)) goto parse_$next_name$;\n",
          "next_tag", SimpleItoa(WireFormat::MakeTag(next_field)),
          "next_name", next_field->name());
      } else {
        // Expect EOF.
        // TODO(kenton):  Expect group end-tag?
        printer->Print(
          "if (input->ExpectAtEnd()) return true;\n");
      }

      printer->Print(
        "break;\n");

      printer->Outdent();
      printer->Print("}\n\n");
    }

    printer->Print(
      "default: {\n"
      "handle_uninterpreted:\n");
    printer->Indent();
  }

  // Is this an end-group tag?  If so, this must be the end of the message.
  printer->Print(
    "if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==\n"
    "    ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {\n"
    "  return true;\n"
    "}\n");

  // Handle extension ranges.
  if (descriptor_->extension_range_count() > 0) {
    printer->Print(
      "if (");
    for (int i = 0; i < descriptor_->extension_range_count(); i++) {
      const Descriptor::ExtensionRange* range =
        descriptor_->extension_range(i);
      if (i > 0) printer->Print(" ||\n    ");

      uint32 start_tag = WireFormatLite::MakeTag(
        range->start, static_cast<WireFormatLite::WireType>(0));
      uint32 end_tag = WireFormatLite::MakeTag(
        range->end, static_cast<WireFormatLite::WireType>(0));

      if (range->end > FieldDescriptor::kMaxNumber) {
        printer->Print(
          "($start$u <= tag)",
          "start", SimpleItoa(start_tag));
      } else {
        printer->Print(
          "($start$u <= tag && tag < $end$u)",
          "start", SimpleItoa(start_tag),
          "end", SimpleItoa(end_tag));
      }
    }
    printer->Print(") {\n");
    if (HasUnknownFields(descriptor_->file())) {
      PrintHandlingOptionalStaticInitializers(
        descriptor_->file(), printer,
        // With static initializers.
        "  DO_(_extensions_.ParseField(tag, input, default_instance_,\n"
        "                              mutable_unknown_fields()));\n",
        // Without.
        "  DO_(_extensions_.ParseField(tag, input, &default_instance(),\n"
        "                              mutable_unknown_fields()));\n");
    } else {
      PrintHandlingOptionalStaticInitializers(
        descriptor_->file(), printer,
        // With static initializers.
        "  DO_(_extensions_.ParseField(tag, input, default_instance_));\n",
        // Without.
        "  DO_(_extensions_.ParseField(tag, input, &default_instance()));\n");
    }
    printer->Print(
      "  continue;\n"
      "}\n");
  }

  // We really don't recognize this tag.  Skip it.
  if (HasUnknownFields(descriptor_->file())) {
    printer->Print(
      "DO_(::google::protobuf::internal::WireFormat::SkipField(\n"
      "      input, tag, mutable_unknown_fields()));\n");
  } else {
    printer->Print(
      "DO_(::google::protobuf::internal::WireFormatLite::SkipField(input, tag));\n");
  }

  if (descriptor_->field_count() > 0) {
    printer->Print("break;\n");
    printer->Outdent();
    printer->Print("}\n");    // default:
    printer->Outdent();
    printer->Print("}\n");    // switch
  }

  printer->Outdent();
  printer->Outdent();
  printer->Print(
    "  }\n"                   // while
    "  return true;\n"
    "#undef DO_\n"
    "}\n");
}

void MessageGenerator::GenerateSerializeOneField(
    io::Printer* printer, const FieldDescriptor* field, bool to_array) {
  PrintFieldComment(printer, field);

  if (!field->is_repeated()) {
    printer->Print(
      "if (has_$name$()) {\n",
      "name", FieldName(field));
    printer->Indent();
  }

  if (to_array) {
    field_generators_.get(field).GenerateSerializeWithCachedSizesToArray(
        printer);
  } else {
    field_generators_.get(field).GenerateSerializeWithCachedSizes(printer);
  }

  if (!field->is_repeated()) {
    printer->Outdent();
    printer->Print("}\n");
  }
  printer->Print("\n");
}

void MessageGenerator::GenerateSerializeOneExtensionRange(
    io::Printer* printer, const Descriptor::ExtensionRange* range,
    bool to_array) {
  map<string, string> vars;
  vars["start"] = SimpleItoa(range->start);
  vars["end"] = SimpleItoa(range->end);
  printer->Print(vars,
    "// Extension range [$start$, $end$)\n");
  if (to_array) {
    printer->Print(vars,
      "target = _extensions_.SerializeWithCachedSizesToArray(\n"
      "    $start$, $end$, target);\n\n");
  } else {
    printer->Print(vars,
      "_extensions_.SerializeWithCachedSizes(\n"
      "    $start$, $end$, output);\n\n");
  }
}

void MessageGenerator::
GenerateSerializeWithCachedSizes(io::Printer* printer) {
  if (descriptor_->options().message_set_wire_format()) {
    // Special-case MessageSet.
    printer->Print(
      "void $classname$::SerializeWithCachedSizes(\n"
      "    ::google::protobuf::io::CodedOutputStream* output) const {\n"
      "  _extensions_.SerializeMessageSetWithCachedSizes(output);\n",
      "classname", classname_);
    if (HasUnknownFields(descriptor_->file())) {
      printer->Print(
        "  ::google::protobuf::internal::WireFormat::SerializeUnknownMessageSetItems(\n"
        "      unknown_fields(), output);\n");
    }
    printer->Print(
      "}\n");
    return;
  }

  printer->Print(
    "void $classname$::SerializeWithCachedSizes(\n"
    "    ::google::protobuf::io::CodedOutputStream* output) const {\n",
    "classname", classname_);
  printer->Indent();

  GenerateSerializeWithCachedSizesBody(printer, false);

  printer->Outdent();
  printer->Print(
    "}\n");
}

void MessageGenerator::
GenerateSerializeWithCachedSizesToArray(io::Printer* printer) {
  if (descriptor_->options().message_set_wire_format()) {
    // Special-case MessageSet.
    printer->Print(
      "::google::protobuf::uint8* $classname$::SerializeWithCachedSizesToArray(\n"
      "    ::google::protobuf::uint8* target) const {\n"
      "  target =\n"
      "      _extensions_.SerializeMessageSetWithCachedSizesToArray(target);\n",
      "classname", classname_);
    if (HasUnknownFields(descriptor_->file())) {
      printer->Print(
        "  target = ::google::protobuf::internal::WireFormat::\n"
        "             SerializeUnknownMessageSetItemsToArray(\n"
        "               unknown_fields(), target);\n");
    }
    printer->Print(
      "  return target;\n"
      "}\n");
    return;
  }

  printer->Print(
    "::google::protobuf::uint8* $classname$::SerializeWithCachedSizesToArray(\n"
    "    ::google::protobuf::uint8* target) const {\n",
    "classname", classname_);
  printer->Indent();

  GenerateSerializeWithCachedSizesBody(printer, true);

  printer->Outdent();
  printer->Print(
    "  return target;\n"
    "}\n");
}

void MessageGenerator::
GenerateSerializeWithCachedSizesBody(io::Printer* printer, bool to_array) {
  scoped_array<const FieldDescriptor*> ordered_fields(
    SortFieldsByNumber(descriptor_));

  vector<const Descriptor::ExtensionRange*> sorted_extensions;
  for (int i = 0; i < descriptor_->extension_range_count(); ++i) {
    sorted_extensions.push_back(descriptor_->extension_range(i));
  }
  sort(sorted_extensions.begin(), sorted_extensions.end(),
       ExtensionRangeSorter());

  // Merge the fields and the extension ranges, both sorted by field number.
  int i, j;
  for (i = 0, j = 0;
       i < descriptor_->field_count() || j < sorted_extensions.size();
       ) {
    if (i == descriptor_->field_count()) {
      GenerateSerializeOneExtensionRange(printer,
                                         sorted_extensions[j++],
                                         to_array);
    } else if (j == sorted_extensions.size()) {
      GenerateSerializeOneField(printer, ordered_fields[i++], to_array);
    } else if (ordered_fields[i]->number() < sorted_extensions[j]->start) {
      GenerateSerializeOneField(printer, ordered_fields[i++], to_array);
    } else {
      GenerateSerializeOneExtensionRange(printer,
                                         sorted_extensions[j++],
                                         to_array);
    }
  }

  if (HasUnknownFields(descriptor_->file())) {
    printer->Print("if (!unknown_fields().empty()) {\n");
    printer->Indent();
    if (to_array) {
      printer->Print(
        "target = "
            "::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(\n"
        "    unknown_fields(), target);\n");
    } else {
      printer->Print(
        "::google::protobuf::internal::WireFormat::SerializeUnknownFields(\n"
        "    unknown_fields(), output);\n");
    }
    printer->Outdent();

    printer->Print(
      "}\n");
  }
}

void MessageGenerator::
GenerateByteSize(io::Printer* printer) {
  if (descriptor_->options().message_set_wire_format()) {
    // Special-case MessageSet.
    printer->Print(
      "int $classname$::ByteSize() const {\n"
      "  int total_size = _extensions_.MessageSetByteSize();\n",
      "classname", classname_);
    if (HasUnknownFields(descriptor_->file())) {
      printer->Print(
        "  total_size += ::google::protobuf::internal::WireFormat::\n"
        "      ComputeUnknownMessageSetItemsSize(unknown_fields());\n");
    }
    printer->Print(
      "  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();\n"
      "  _cached_size_ = total_size;\n"
      "  GOOGLE_SAFE_CONCURRENT_WRITES_END();\n"
      "  return total_size;\n"
      "}\n");
    return;
  }

  printer->Print(
    "int $classname$::ByteSize() const {\n",
    "classname", classname_);
  printer->Indent();
  printer->Print(
    "int total_size = 0;\n"
    "\n");

  int last_index = -1;

  for (int i = 0; i < descriptor_->field_count(); i++) {
    const FieldDescriptor* field = descriptor_->field(i);

    if (!field->is_repeated()) {
      // See above in GenerateClear for an explanation of this.
      // TODO(kenton):  Share code?  Unclear how to do so without
      //   over-engineering.
      if ((i / 8) != (last_index / 8) ||
          last_index < 0) {
        if (last_index >= 0) {
          printer->Outdent();
          printer->Print("}\n");
        }
        printer->Print(
          "if (_has_bits_[$index$ / 32] & (0xffu << ($index$ % 32))) {\n",
          "index", SimpleItoa(field->index()));
        printer->Indent();
      }
      last_index = i;

      PrintFieldComment(printer, field);

      printer->Print(
        "if (has_$name$()) {\n",
        "name", FieldName(field));
      printer->Indent();

      field_generators_.get(field).GenerateByteSize(printer);

      printer->Outdent();
      printer->Print(
        "}\n"
        "\n");
    }
  }

  if (last_index >= 0) {
    printer->Outdent();
    printer->Print("}\n");
  }

  // Repeated fields don't use _has_bits_ so we count them in a separate
  // pass.
  for (int i = 0; i < descriptor_->field_count(); i++) {
    const FieldDescriptor* field = descriptor_->field(i);

    if (field->is_repeated()) {
      PrintFieldComment(printer, field);
      field_generators_.get(field).GenerateByteSize(printer);
      printer->Print("\n");
    }
  }

  if (descriptor_->extension_range_count() > 0) {
    printer->Print(
      "total_size += _extensions_.ByteSize();\n"
      "\n");
  }

  if (HasUnknownFields(descriptor_->file())) {
    printer->Print("if (!unknown_fields().empty()) {\n");
    printer->Indent();
    printer->Print(
      "total_size +=\n"
      "  ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(\n"
      "    unknown_fields());\n");
    printer->Outdent();
    printer->Print("}\n");
  }

  // We update _cached_size_ even though this is a const method.  In theory,
  // this is not thread-compatible, because concurrent writes have undefined
  // results.  In practice, since any concurrent writes will be writing the
  // exact same value, it works on all common processors.  In a future version
  // of C++, _cached_size_ should be made into an atomic<int>.
  printer->Print(
    "GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();\n"
    "_cached_size_ = total_size;\n"
    "GOOGLE_SAFE_CONCURRENT_WRITES_END();\n"
    "return total_size;\n");

  printer->Outdent();
  printer->Print("}\n");
}

void MessageGenerator::
GenerateIsInitialized(io::Printer* printer) {
  printer->Print(
    "bool $classname$::IsInitialized() const {\n",
    "classname", classname_);
  printer->Indent();

  // Check that all required fields in this message are set.  We can do this
  // most efficiently by checking 32 "has bits" at a time.
  int has_bits_array_size = (descriptor_->field_count() + 31) / 32;
  for (int i = 0; i < has_bits_array_size; i++) {
    uint32 mask = 0;
    for (int bit = 0; bit < 32; bit++) {
      int index = i * 32 + bit;
      if (index >= descriptor_->field_count()) break;
      const FieldDescriptor* field = descriptor_->field(index);

      if (field->is_required()) {
        mask |= 1 << bit;
      }
    }

    if (mask != 0) {
      char buffer[kFastToBufferSize];
      printer->Print(
        "if ((_has_bits_[$i$] & 0x$mask$) != 0x$mask$) return false;\n",
        "i", SimpleItoa(i),
        "mask", FastHex32ToBuffer(mask, buffer));
    }
  }

  // Now check that all embedded messages are initialized.
  printer->Print("\n");
  for (int i = 0; i < descriptor_->field_count(); i++) {
    const FieldDescriptor* field = descriptor_->field(i);
    if (field->cpp_type() == FieldDescriptor::CPPTYPE_MESSAGE &&
        !ShouldIgnoreRequiredFieldCheck(field) &&
        HasRequiredFields(field->message_type())) {
      if (field->is_repeated()) {
        printer->Print(
          "for (int i = 0; i < $name$_size(); i++) {\n"
          "  if (!this->$name$(i).IsInitialized()) return false;\n"
          "}\n",
          "name", FieldName(field));
      } else {
        printer->Print(
          "if (has_$name$()) {\n"
          "  if (!this->$name$().IsInitialized()) return false;\n"
          "}\n",
          "name", FieldName(field));
      }
    }
  }

  if (descriptor_->extension_range_count() > 0) {
    printer->Print(
      "\n"
      "if (!_extensions_.IsInitialized()) return false;");
  }

  printer->Outdent();
  printer->Print(
    "  return true;\n"
    "}\n");
}


}  // namespace cpp
}  // namespace compiler
}  // namespace protobuf
}  // namespace google
