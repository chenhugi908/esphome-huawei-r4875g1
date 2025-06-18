# ESPHome Huawei R4875G1 Rectifier Module Component

Custom ESPHome component for controlling Huawei R4875G1 Rectifier Modules.

## Features
- Full control of Huawei R4875G1 rectifier
- Support for up to 100V output (requires hardware modification)
- Individual sensor calibration
- CAN bus communication at 125kbps
- Home Assistant integration

## Installation
Add to your ESPHome configuration:

```yaml
external_components:
  - source: github://your_username/esphome-huawei-r4875g1
    components: [ huawei_r4875g1 ]