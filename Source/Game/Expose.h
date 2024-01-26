#pragma once
#include "JsonManager.h"
#include <Engine\CommonUtilities.h>

#define JSMCR_EXPOSE_ARG_1(jxm_var) \
Singleton<JsonManager>().ExposeVariableMacro(&jxm_var, #jxm_var, "", __FILE__)

#define JSMCR_EXPOSE_ARG_2(jxm_var, jxm_name) \
Singleton<JsonManager>().ExposeVariableMacro(&jxm_var, #jxm_var, jxm_name, __FILE__)

#define JSMCR_EXPOSE_ARG_3(jxm_var, jxm_name, jxm_special) \
Singleton<JsonManager>().ExposeVariableMacro(&jxm_var, #jxm_var, jxm_name, __FILE__, jxm_special)

#define JSMCR_EXPAND(jxm_args) jxm_args
#define JSMCR_DEDUCE_MACRO(jxm_3, jxm_2, jxm_1, jxm_Name, ...) jxm_Name
#define EXPOSE(...) JSMCR_EXPAND(JSMCR_DEDUCE_MACRO(__VA_ARGS__, \
    JSMCR_EXPOSE_ARG_3, \
    JSMCR_EXPOSE_ARG_2, \
    JSMCR_EXPOSE_ARG_1  \
)(__VA_ARGS__))

#define UNEXPOSE(var) \
Singleton<JsonManager>().Unexpose(&var)
