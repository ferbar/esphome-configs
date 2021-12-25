# ESPHome Configs

## Addressable Light Examples
=> config/addressable_effects.yaml

<img src="christbaum_2021.gif" alt="Addressable light example" width="240"/>

## Memory Map generieren:

esphome -v compile x.yaml

## Logs anzeigen
minicom:

ctrl A - N => Timestamps anzeigen

minicom -R UTF-8 => UTF8 Zeichen werden richtig angezeigt, dafür sind die Umlaute in den Menüs kaputt

## example configs:

### vom HA aus änderbare Variable:
```
number:
  - platform: template
    name: "SDS011 Update Interval"
    min_value: 1
    max_value: 100
    step: 1
    lambda: |-
      float value = id(pm).update_interval_min_;
      ESP_LOGD("main", "Returning ********************************** %f ****************", value);
      return value;
    set_action:
      lambda: |-
        ESP_LOGD("main", "Setting ********************************** %f ****************", x);
        id(pm).update_interval_min_=x;
```
oder statt lambda + set action in der lambda func id(number_id) übergeben und mit add_on_state_callback() auf änderungen listen
```
custom_component:
- lambda: |-
    auto myComponent = new AdvancedBlinkComponent(id(blink_enable), id(blink_frequency));
    return {myComponent};
```

```
  AdvancedBlinkComponent(esphome::template_::TemplateSwitch *&_enable, esphome::template_::TemplateNumber *&_frequency)
  {
    _enable->add_on_state_callback([this](bool newState)
                                   { enable = newState; });
```

Siehe:
https://loopylab.de/2021/10/10/arduino-to-esphome.html
