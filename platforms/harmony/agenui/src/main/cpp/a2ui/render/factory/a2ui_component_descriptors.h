#pragma once

#include <memory>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

namespace agenui {
class IMeasurement;
}

namespace a2ui {

class A2UIComponent;

/**
 * Creates a native component instance. The caller owns the returned pointer.
 */
using ComponentCreateFn = A2UIComponent* (*)(const std::string& id,
                                             const nlohmann::json& properties);

/**
 * Returns the shared measurement instance for a component type.
 * Nullptr in a descriptor means the type has no native measurement.
 */
using MeasurementFactoryFn = std::shared_ptr<agenui::IMeasurement> (*)();

/**
 * Static descriptor of one built-in component type.
 *
 * This table is the single seed for the global ComponentRegistry and the
 * built-in measurement list: adding a new built-in component only requires
 * writing the component class plus one row here. Runtime (host) components
 * are registered into the same registry through the NAPI registerComponent
 * export instead of this table.
 */
struct ComponentDescriptor {
    /// DSL type name, e.g. "Text". Matches the a2ui_component_types.h constants.
    std::string type;
    /// True when creation is delegated to the ArkTS hybrid channel.
    bool isHybrid = false;
    /// Native creator; nullptr for hybrid types.
    ComponentCreateFn create = nullptr;
    /// Native measurement; nullptr when the type has none.
    MeasurementFactoryFn measurement = nullptr;
};

/**
 * All built-in component descriptors (single source of truth).
 */
const std::vector<ComponentDescriptor>& getComponentDescriptors();

/**
 * Look up a descriptor by type name.
 * @return Matching descriptor, or nullptr when the type is not built-in.
 */
const ComponentDescriptor* findComponentDescriptor(const std::string& type);

} // namespace a2ui
