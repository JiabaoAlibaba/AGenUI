#include "a2ui_component_descriptors.h"
#include "../a2ui_component_types.h"

// Component headers
#include "../components/text_component.h"
#include "../components/image_component.h"
#include "../components/button_component.h"
#include "../components/icon_component.h"
#include "../components/divider_component.h"
#include "../components/video_component.h"
#include "../components/audio_player_component.h"
#include "../components/modal_component.h"
#include "../components/column_component.h"
#include "../components/row_component.h"
#include "../components/card_component.h"
#include "../components/tabs_component.h"
#include "../components/list_component.h"
#include "../components/textfield_component.h"
#include "../components/checkbox_component.h"
#include "../components/slider_component.h"
#include "../components/choicepicker_component.h"
#include "../components/datetimeinput_component.h"
#include "../components/richtext_component.h"
#include "../components/table_component.h"
#include "../components/carousel_component.h"

// Measurement headers
#include "agenui_measurement.h"
#include "a2ui/measure/image_component_measurement.h"
#include "a2ui/measure/slider_component_measurement.h"
#include "a2ui/measure/text_component_measurement.h"
#include "a2ui/measure/checkbox_component_measurement.h"
#include "a2ui/measure/choice_picker_component_measurement.h"
#include "a2ui/measure/table_component_measurement.h"
#include "a2ui/measure/tabs_component_measurement.h"
#include "a2ui/measure/datetimeinput_component_measurement.h"
#include "a2ui/measure/divider_component_measurement.h"
#include "a2ui/measure/audioplayer_component_measurement.h"

namespace a2ui {

namespace {

template <typename T>
A2UIComponent* createNative(const std::string& id, const nlohmann::json& properties) {
    return new T(id, properties);
}

// Stateless measurements are shared singletons across types that reuse them
// (Text/RichText, Image/Icon).

std::shared_ptr<agenui::IMeasurement> textMeasurement() {
    static std::shared_ptr<agenui::IMeasurement> m = std::make_shared<TextComponentMeasurement>();
    return m;
}

std::shared_ptr<agenui::IMeasurement> imageMeasurement() {
    static std::shared_ptr<agenui::IMeasurement> m = std::make_shared<ImageComponentMeasurement>();
    return m;
}

std::shared_ptr<agenui::IMeasurement> sliderMeasurement() {
    static std::shared_ptr<agenui::IMeasurement> m = std::make_shared<SliderComponentMeasurement>();
    return m;
}

std::shared_ptr<agenui::IMeasurement> checkBoxMeasurement() {
    static std::shared_ptr<agenui::IMeasurement> m = std::make_shared<CheckBoxComponentMeasurement>();
    return m;
}

std::shared_ptr<agenui::IMeasurement> choicePickerMeasurement() {
    static std::shared_ptr<agenui::IMeasurement> m = std::make_shared<ChoicePickerComponentMeasurement>();
    return m;
}

std::shared_ptr<agenui::IMeasurement> tableMeasurement() {
    static std::shared_ptr<agenui::IMeasurement> m = std::make_shared<TableComponentMeasurement>();
    return m;
}

std::shared_ptr<agenui::IMeasurement> tabsMeasurement() {
    static std::shared_ptr<agenui::IMeasurement> m = std::make_shared<TabsComponentMeasurement>();
    return m;
}

std::shared_ptr<agenui::IMeasurement> dateTimeInputMeasurement() {
    static std::shared_ptr<agenui::IMeasurement> m = std::make_shared<DateTimeInputComponentMeasurement>();
    return m;
}

std::shared_ptr<agenui::IMeasurement> dividerMeasurement() {
    static std::shared_ptr<agenui::IMeasurement> m = std::make_shared<DividerComponentMeasurement>();
    return m;
}

std::shared_ptr<agenui::IMeasurement> audioPlayerMeasurement() {
    static std::shared_ptr<agenui::IMeasurement> m = std::make_shared<AudioPlayerComponentMeasurement>();
    return m;
}

} // namespace

const std::vector<ComponentDescriptor>& getComponentDescriptors() {
    static const std::vector<ComponentDescriptor> kDescriptors = {
        // Base components
        { ComponentType::kText,         false, &createNative<TextComponent>,        &textMeasurement },
        { ComponentType::kButton,       false, &createNative<ButtonComponent>,      nullptr },
        { ComponentType::kImage,        false, &createNative<ImageComponent>,       &imageMeasurement },
        { ComponentType::kIcon,         false, &createNative<IconComponent>,        &imageMeasurement },
        { ComponentType::kDivider,      false, &createNative<DividerComponent>,     &dividerMeasurement },
        { ComponentType::kVideo,        false, &createNative<VideoComponent>,       nullptr },
        { ComponentType::kAudioPlayer,  false, &createNative<AudioPlayerComponent>, &audioPlayerMeasurement },
        { ComponentType::kModal,        false, &createNative<ModalComponent>,       nullptr },

        // Layout components
        { ComponentType::kColumn,       false, &createNative<ColumnComponent>,      nullptr },
        { ComponentType::kRow,          false, &createNative<RowComponent>,         nullptr },
        { ComponentType::kCard,         false, &createNative<CardComponent>,        nullptr },
        { ComponentType::kTabs,         false, &createNative<TabsComponent>,        &tabsMeasurement },
        { ComponentType::kList,         false, &createNative<ListComponent>,        nullptr },

        // Interactive components
        { ComponentType::kTextField,    false, &createNative<TextFieldComponent>,    nullptr },
        { ComponentType::kCheckBox,     false, &createNative<CheckBoxComponent>,     &checkBoxMeasurement },
        { ComponentType::kSlider,       false, &createNative<SliderComponent>,       &sliderMeasurement },
        { ComponentType::kChoicePicker, false, &createNative<ChoicePickerComponent>, &choicePickerMeasurement },
        { ComponentType::kDateTimeInput,false, &createNative<DateTimeInputComponent>,&dateTimeInputMeasurement },

        // Extended components
        { ComponentType::kRichText,     false, &createNative<RichTextComponent>,    &textMeasurement },
        // NOTE: "AmapText" is intentionally NOT listed here. The amap host
        // registers an ArkTS hybrid component under this type name at runtime
        // (NAPI registerComponent) to provide SpanText rich-text rendering
        // (text/spans dual mode), together with its own SpanText-aware
        // measurement. Registering a native TextComponentMeasurement here
        // would only serve as a misleading fallback that does not understand
        // `spans`.
        { ComponentType::kWeb,          true,  nullptr,                             nullptr }, // built-in hybrid: creation goes through the ArkTS channel
        { ComponentType::kTable,        false, &createNative<TableComponent>,       &tableMeasurement },
        { ComponentType::kCarousel,     false, &createNative<CarouselComponent>,    nullptr },
    };
    return kDescriptors;
}

const ComponentDescriptor* findComponentDescriptor(const std::string& type) {
    for (const auto& descriptor : getComponentDescriptors()) {
        if (descriptor.type == type) {
            return &descriptor;
        }
    }
    return nullptr;
}

} // namespace a2ui
