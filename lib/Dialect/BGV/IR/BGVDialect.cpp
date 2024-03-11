#include "include/Dialect/BGV/IR/BGVDialect.h"

#include "include/Dialect/BGV/IR/BGVOps.h"
#include "llvm/include/llvm/ADT/TypeSwitch.h"            // from @llvm-project
#include "mlir/include/mlir/IR/Builders.h"               // from @llvm-project
#include "mlir/include/mlir/IR/DialectImplementation.h"  // from @llvm-project

// Generated definitions
#include "include/Dialect/BGV/IR/BGVDialect.cpp.inc"
#define GET_OP_CLASSES
#include "include/Dialect/BGV/IR/BGVOps.cpp.inc"

namespace mlir {
namespace heir {
namespace bgv {

//===----------------------------------------------------------------------===//
// BGV dialect.
//===----------------------------------------------------------------------===//

// Dialect construction: there is one instance per context and it registers its
// operations, types, and interfaces here.
void BGVDialect::initialize() {
  addOperations<
#define GET_OP_LIST
#include "include/Dialect/BGV/IR/BGVOps.cpp.inc"
      >();
}

LogicalResult MulOp::verify() {
  auto x = getX().getType();
  auto y = getY().getType();
  if (x.getRlweParams().getDimension() != y.getRlweParams().getDimension()) {
    return emitOpError() << "input dimensions do not match";
  }
  auto out = getOutput().getType();
  if (out.getRlweParams().getDimension() !=
      1 + x.getRlweParams().getDimension()) {
    return emitOpError() << "output.dim == x.dim + 1 does not hold";
  }
  return success();
}
LogicalResult Rotate::verify() {
  auto x = getX().getType();
  if (x.getRlweParams().getDimension() != 2) {
    return emitOpError() << "x.dim == 2 does not hold";
  }
  auto out = getOutput().getType();
  if (out.getRlweParams().getDimension() != 2) {
    return emitOpError() << "output.dim == 2 does not hold";
  }
  return success();
}

LogicalResult Relinearize::verify() {
  auto x = getX().getType();
  auto out = getOutput().getType();
  if (x.getRlweParams().getDimension() != getFromBasis().size()) {
    return emitOpError() << "input dimension does not match from_basis";
  }
  if (out.getRlweParams().getDimension() != getToBasis().size()) {
    return emitOpError() << "output dimension does not match to_basis";
  }
  return success();
}

LogicalResult ModulusSwitch::verify() {
  auto x = getX().getType();
  auto xRing = x.getRlweParams().getRing();

  auto out = getOutput().getType();
  auto outRing = out.getRlweParams().getRing();
  if (outRing != getToRing()) {
    return emitOpError() << "output ring should match to_ring";
  }
  if (xRing.getCmod().getValue().ule(outRing.getCmod().getValue())) {
    return emitOpError()
           << "output ring modulus should be less than the input ring modulus";
  }
  if (!xRing.getCmod().getValue().urem(outRing.getCmod().getValue()).isZero()) {
    return emitOpError()
           << "output ring modulus should divide the input ring modulus";
  }

  return success();
}

}  // namespace bgv
}  // namespace heir
}  // namespace mlir
