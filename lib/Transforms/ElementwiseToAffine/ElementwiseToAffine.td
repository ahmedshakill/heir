#ifndef LIB_TRANSFORMS_ELEMENTWISETOAFFINE_ELEMENTWISETOAFFINE_TD_
#define LIB_TRANSFORMS_ELEMENTWISETOAFFINE_ELEMENTWISETOAFFINE_TD_

include "mlir/Pass/PassBase.td"

def ElementwiseToAffine : Pass<"convert-elementwise-to-affine"> {
  let summary = "This pass lowers ElementwiseMappable operations to Affine loops.";
  let description = [{
    This pass lowers ElementwiseMappable operations over tensors
    to affine loop nests that instead apply the operation to the underlying scalar values.
  }];
  let dependentDialects = [
    "mlir::affine::AffineDialect",
    "mlir::tensor::TensorDialect"
  ];
  let options = [
    ListOption<"dialectsToConsider","dialects-to-consider", "std::string",
                "comma-separated list of dialects to run this pass on ">,
    ListOption<"opsToConsider","ops-to-consider", "std::string",
                "comma-separated list of ops to run this pass on ">
  ];
}

#endif  // LIB_TRANSFORMS_ELEMENTWISETOAFFINE_ELEMENTWISETOAFFINE_TD_
