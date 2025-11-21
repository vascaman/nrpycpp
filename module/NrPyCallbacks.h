#ifndef PYCALLBACKS_H
#define PYCALLBACKS_H
#pragma once
#define QT_ANNOTATE_ACCESS_SPECIFIER(a) __attribute__((annotate(#a)))
//The special **QT_ANNOTATE_ACCESS_SPECIFIER** line ensures that signals and slots are visible to the generator.
//(Since this header file is only used by the generator, it’s not harmful to leave it in even if you don’t need it.)
#include "../lib/PyCallback.h"
#endif // PYCALLBACKS_H

