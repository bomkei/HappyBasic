#include "main.h"

PrgCtx* PrgCtx::Instance;

ProgramContext::ProgramContext() {
  Instance = this;
}
