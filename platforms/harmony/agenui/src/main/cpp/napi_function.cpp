#include "napi_internal.h"
#include "a2ui/bridge/open_url_helper.h"
#include "a2ui/bridge/skill_invoker_helper.h"
#include "a2ui/bridge/harmony_platform_function.h"
#include "a2ui/render/factory/a2ui_component_registry.h"

napi_value RegisterOpenUrlCallback(napi_env env, napi_callback_info info) {
    size_t argc = 1;
    napi_value args[1];
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);

    if (argc < 1) {
        HM_LOGE("RegisterOpenUrlCallback: Invalid argument count");
        NAPI_RETURN_UNDEFINED(env);
    }

    if (!napiCheckArgIsFunction(env, args[0], "RegisterOpenUrlCallback")) {
        NAPI_RETURN_UNDEFINED(env);
    }

    a2ui::OpenUrlHelper::getInstance().registerCallback(env, args[0]);
    HM_LOGI("RegisterOpenUrlCallback: Callback registered successfully");

    NAPI_RETURN_UNDEFINED(env);
}

napi_value RegisterSkillInvokerCallback(napi_env env, napi_callback_info info) {
    size_t argc = 1;
    napi_value args[1];
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);

    if (argc < 1) {
        HM_LOGE("RegisterSkillInvokerCallback: Invalid argument count");
        NAPI_RETURN_UNDEFINED(env);
    }

    if (!napiCheckArgIsFunction(env, args[0], "RegisterSkillInvokerCallback")) {
        NAPI_RETURN_UNDEFINED(env);
    }

    a2ui::SkillInvokerHelper::getInstance().registerCallback(env, args[0]);
    HM_LOGI("RegisterSkillInvokerCallback: Callback registered successfully");

    NAPI_RETURN_UNDEFINED(env);
}

napi_value RegisterEtsFunction(napi_env env, napi_callback_info info) {
    HM_LOGE("RegisterEtsFunction: invoked");
    size_t argc = 2;
    napi_value args[2];
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);

    if (argc < 2) {
        HM_LOGE("RegisterEtsFunction: Expected 2 arguments, got %zu", argc);
        NAPI_RETURN_UNDEFINED(env);
    }

    std::string name = napiGetString(env, args[0]);

    if (!napiCheckArgIsFunction(env, args[1], "RegisterEtsFunction (second arg)")) {
        NAPI_RETURN_UNDEFINED(env);
    }

    a2ui::registerEtsFunction(name, env, args[1]);

    NAPI_RETURN_UNDEFINED(env);
}

napi_value RegisterFunction(napi_env env, napi_callback_info info) {
    size_t argc = 3;
    napi_value args[3];
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);

    if (argc < 3) {
        HM_LOGE("RegisterFunction: Expected 3 arguments, got %{public}zu", argc);
        NAPI_RETURN_UNDEFINED(env);
    }

    std::string name = napiGetString(env, args[0]);
    std::string config = napiGetString(env, args[1]);

    if (!napiCheckArgIsFunction(env, args[2], "RegisterFunction (third arg)")) {
        NAPI_RETURN_UNDEFINED(env);
    }

    auto* engine = agenui::getAGenUIEngine();
    if (!engine) {
        HM_LOGE("RegisterFunction: Engine not initialized");
        NAPI_RETURN_UNDEFINED(env);
    }

    {
        std::lock_guard<std::mutex> lock(g_platformFunctionsMutex);
        auto it = getPlatformFunctions().find(name);
        if (it != getPlatformFunctions().end()) {
            engine->unregisterFunction(name);
            getPlatformFunctions().erase(it);
        }
    }

    auto function = std::make_unique<agenui::HarmonyPlatformFunction>(env, args[2], g_mainTsfn, g_mainThreadId);
    if (!function->isValid()) {
        HM_LOGE("RegisterFunction: Failed to create HarmonyPlatformFunction for %s", name.c_str());
        NAPI_RETURN_UNDEFINED(env);
    }

    engine->registerFunction(config, function.get());

    {
        std::lock_guard<std::mutex> lock(g_platformFunctionsMutex);
        getPlatformFunctions()[name] = std::move(function);
    }

    HM_LOGI("RegisterFunction: name=%s registered successfully", name.c_str());

    NAPI_RETURN_UNDEFINED(env);
}

napi_value UnregisterFunction(napi_env env, napi_callback_info info) {
    size_t argc = 1;
    napi_value args[1];
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);

    if (argc < 1) {
        HM_LOGE("UnregisterFunction: Expected 1 argument");
        NAPI_RETURN_UNDEFINED(env);
    }

    std::string name = napiGetString(env, args[0]);

    auto* engine = agenui::getAGenUIEngine();
    if (!engine) {
        HM_LOGE("UnregisterFunction: Engine not initialized");
        NAPI_RETURN_UNDEFINED(env);
    }

    engine->unregisterFunction(name);

    {
        std::lock_guard<std::mutex> lock(g_platformFunctionsMutex);
        auto it = getPlatformFunctions().find(name);
        if (it != getPlatformFunctions().end()) {
            getPlatformFunctions().erase(it);
            HM_LOGI("UnregisterFunction: destroyed HarmonyPlatformFunction for %s", name.c_str());
        }
    }

    NAPI_RETURN_UNDEFINED(env);
}

napi_value RegisterComponent(napi_env env, napi_callback_info info) {
    size_t argc = 1;
    napi_value args[1];
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);

    if (argc < 1) {
        HM_LOGE("RegisterComponent: Expected 1 argument, got %zu", argc);
        NAPI_RETURN_UNDEFINED(env);
    }

    std::string type = napiGetString(env, args[0]);
    if (type.empty()) {
        HM_LOGE("RegisterComponent: component type is empty");
        NAPI_RETURN_UNDEFINED(env);
    }

    // Host components are always hybrid: creation is delegated to the ArkTS
    // side through the HybridView channel (view creators are registered
    // separately via HybridViewManager).
    a2ui::ComponentDescriptor descriptor;
    descriptor.type = type;
    descriptor.isHybrid = true;
    a2ui::ComponentRegistry::global().registerComponent(type, descriptor);

    HM_LOGI("RegisterComponent: type=%s registered as hybrid", type.c_str());
    NAPI_RETURN_UNDEFINED(env);
}

napi_value UnregisterComponent(napi_env env, napi_callback_info info) {
    size_t argc = 1;
    napi_value args[1];
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);

    if (argc < 1) {
        HM_LOGE("UnregisterComponent: Expected 1 argument, got %zu", argc);
        NAPI_RETURN_UNDEFINED(env);
    }

    std::string type = napiGetString(env, args[0]);
    a2ui::ComponentRegistry::global().unregisterComponent(type);

    HM_LOGI("UnregisterComponent: type=%s", type.c_str());
    NAPI_RETURN_UNDEFINED(env);
}
