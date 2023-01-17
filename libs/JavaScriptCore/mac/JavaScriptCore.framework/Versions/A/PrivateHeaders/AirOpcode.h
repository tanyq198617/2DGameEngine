// Generated by opcode_generator.rb from JavaScriptCore/b3/air/AirOpcode.opcodes -- do not edit!
#ifndef AirOpcode_h
#define AirOpcode_h
namespace JSC { namespace B3 { namespace Air {
enum Opcode : int16_t {
    AbsDouble,
    AbsFloat,
    Add16,
    Add32,
    Add64,
    Add8,
    AddDouble,
    AddFloat,
    And32,
    And64,
    AndDouble,
    AndFloat,
    Branch32,
    Branch64,
    Branch8,
    BranchAdd32,
    BranchAdd64,
    BranchDouble,
    BranchFloat,
    BranchMul32,
    BranchMul64,
    BranchNeg32,
    BranchNeg64,
    BranchSub32,
    BranchSub64,
    BranchTest32,
    BranchTest64,
    BranchTest8,
    CCall,
    CeilDouble,
    CeilFloat,
    ColdCCall,
    Compare32,
    Compare64,
    CompareDouble,
    CompareFloat,
    ConvertDoubleToFloat,
    ConvertFloatToDouble,
    ConvertInt32ToDouble,
    ConvertInt32ToFloat,
    ConvertInt64ToDouble,
    ConvertInt64ToFloat,
    CountLeadingZeros32,
    CountLeadingZeros64,
    Div32,
    Div64,
    DivDouble,
    DivFloat,
    EntrySwitch,
    FloorDouble,
    FloorFloat,
    Jump,
    Lea32,
    Lea64,
    Load16,
    Load16SignedExtendTo32,
    Load8,
    Load8SignedExtendTo32,
    LoadFence,
    Lshift32,
    Lshift64,
    MemoryFence,
    Move,
    Move32,
    Move32ToFloat,
    Move64ToDouble,
    MoveConditionally32,
    MoveConditionally64,
    MoveConditionallyDouble,
    MoveConditionallyFloat,
    MoveConditionallyTest32,
    MoveConditionallyTest64,
    MoveDouble,
    MoveDoubleConditionally32,
    MoveDoubleConditionally64,
    MoveDoubleConditionallyDouble,
    MoveDoubleConditionallyFloat,
    MoveDoubleConditionallyTest32,
    MoveDoubleConditionallyTest64,
    MoveDoubleTo64,
    MoveFloat,
    MoveFloatTo32,
    MoveZeroToDouble,
    Mul32,
    Mul64,
    MulDouble,
    MulFloat,
    MultiplyAdd32,
    MultiplyAdd64,
    MultiplyNeg32,
    MultiplyNeg64,
    MultiplySub32,
    MultiplySub64,
    Neg32,
    Neg64,
    NegateDouble,
    NegateFloat,
    Nop,
    Not32,
    Not64,
    Oops,
    Or32,
    Or64,
    OrDouble,
    OrFloat,
    Patch,
    Ret32,
    Ret64,
    RetDouble,
    RetFloat,
    RetVoid,
    RotateLeft32,
    RotateLeft64,
    RotateRight32,
    RotateRight64,
    Rshift32,
    Rshift64,
    Shuffle,
    SignExtend16To32,
    SignExtend32ToPtr,
    SignExtend8To32,
    SqrtDouble,
    SqrtFloat,
    Store16,
    Store8,
    StoreFence,
    StoreZero32,
    Sub32,
    Sub64,
    SubDouble,
    SubFloat,
    Swap32,
    Swap64,
    Test32,
    Test64,
    UDiv32,
    UDiv64,
    Urshift32,
    Urshift64,
    WasmBoundsCheck,
    X86ConvertToDoubleWord32,
    X86ConvertToQuadWord64,
    X86Div32,
    X86Div64,
    X86UDiv32,
    X86UDiv64,
    Xor32,
    Xor64,
    XorDouble,
    XorFloat,
    ZeroExtend16To32,
    ZeroExtend8To32,
};
static const unsigned numOpcodes = 152;
} } } // namespace JSC::B3::Air
namespace WTF {
class PrintStream;
JS_EXPORT_PRIVATE void printInternal(PrintStream&, JSC::B3::Air::Opcode);
} // namespace WTF
#endif // AirOpcode_h
