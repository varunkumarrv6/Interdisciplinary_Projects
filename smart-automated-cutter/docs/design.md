# Design

Source: team design report (Google Docs export, Apr 2025). Statements below are the report's unless marked **[check]**.

## 1. Problem and motivation
- Hand cutting of foam, fabric, cardboard, plywood etc. is imprecise; errors accumulate over large areas; sheets sag or shift; large sheets make the whole path hard to see.
- Existing options: manual tools, table/panel saws, laser and plasma cutters. CNC/laser are precise but costly, bulky and need trained operators and CAD input.
- Target users listed: print/packaging SMEs, education/makerspaces, designers and hobbyists, robotics startups, shop operators.
- Wanted: low-cost mobile cutter, vision line-following, compact (A4-A3 sheets), controllable blade depth (about 1-2 mm for layered media like vinyl).

## 2. Design evolution
| Stage | Idea | Why it changed |
|---|---|---|
| I | Hand-sized portable cutter, smooth (toothless) rotating blade, heavy enough to drag | Sheet lift/movement loses accuracy; long runtime needs a big battery; blade must retract and be tilted/fixed |
| II | 4-wheel bot on 18 x 16 cm acrylic plate; 3D-printed motor and HuskyLens holders; servo + linear actuator for blade; buttons for depth | Cost; in-house 3D printing unavailable above 15 cm, so fewer custom printed parts than the "old model" |
| III | HuskyLens line tracking replaces colour and IR sensors | Colour sensor and IR sensors could not identify the thin line (colour sensor: author's recollection, not in report). Report's explanation for IR: needs a line of at least 6.5 mm; target is about 0.4 mm. HuskyLens uses on-board AI vision and detects thin lines |

CAD: [side view with rack/servo](../images/cad/side-view-servo-rack-mount.jpg), [exploded holders](../images/cad/exploded-holders.jpg), [base plate](../images/cad/base-plate.jpg), [top view](../images/cad/top-view-assembly.jpg).
The [early assembly CAD](../images/cad/early-assembly-with-ultrasonic-mount.jpg) shows an ultrasonic-sensor mount that is not in the final component list.

## 3. Components
| Part | Role | Notes from report |
|---|---|---|
| Arduino Uno R3 (ATmega328P, 16 MHz, 2 KB SRAM) | Main controller | |
| HuskyLens (Kendryte K210, 320x240, UART/I2C) | Line detection, returns line coordinates | I2C used in code |
| L293D motor shield | Drives 4 DC motors | 600 mA/channel peak per report |
| 4x DC gear motors + wheels | Drive |  150 RPM  |
| MG996R servo | Blade height via pinion | 4.8-7.2 V, 10-12 kg·cm stall |
| Rack and pinion | Rotation -> linear blade motion | ~2-4 cm stroke, PLA/plastic |
| 3 pushbuttons | Blade height / start | Mapped to A0, A1, A2 in code |
| Battery | 18650 pack (7.4/11.1 V) or 12 V SLA given as examples|
| Chassis | Acrylic plate with 3D-printed holders | 18 x 16 cm (Stage II) |
| Blade | Scalpel/craft-type blade | Fixed to rack |

## 4. Pin map (from firmware)
| Signal | Pin |
|---|---|
| HuskyLens | I2C (A4/A5 on Uno) |
| Servo | D9 |
| Button "cw" -> servo 0 deg | A0 |
| Button "acw" -> servo 180 deg | A1 |
| Button "stop/confirm" | A2 |
| Status LED | D13 |
| Motors | M1-M4 terminals on L293D shield (M3/M4 left, M1/M2 right) |

## 5. Control algorithm
1. Read first line "arrow" from HuskyLens; `error = xTarget - 160`.
2. `correction = Kp*error + Ki*sum + Kd*delta` with Kp=4, Ki=0, Kd=0.
3. `left = 200 + correction`, `right = 200 - correction`, clamped to +/-255. Negative = reverse.
4. No arrow / no result -> release all motors.

## 6. Results
See README. One photographed cut on paper; no quantified accuracy.
