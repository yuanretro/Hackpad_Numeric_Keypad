# HackPad: Numerical Keypad

By YuanRetro (William) Li, 2025.12 - 2026.3

Email: [admin@yuanshen.dev](mailto:admin@yuanshen.dev)

GitHub Repo: <https://github.com/yuanretro/Hackpad_Numeric_Keypad>

Last Revision: 2026-MAR-21

## Introduction

This is my custom variant of [*HackPad*](https://github.com/hackclub/hackpad), an open-source keypad designed by *HackClub*. My project is loosely based on this, but I chose a different path for many elements of my design. Instead of only a few keys like the sample design, I incorporated a full-size computer numpad, very useful for small-size laptops that lack one. For many games I've played, especially simulation games like *Microsoft Flight Simulator* or *SimRail*, the numpad plays a crucial role in the controls, and it would be a pain to play them without one. 

For example, I used *EasyEDA* for the PCB design instead of *KiCAD*, as I have quite a lot of previous experience with it since I designed my first PCB back in 2023. It is also much easier to order PCBs right from the editor, and this makes the whole process less error-prone. I designed my 3D-printable shell initially with *Tinkercad*, but made use of *Autodesk Fusion* for the final touches. I have an Ender 3 V2 printer myself, and I will be able to print the whole casing myself. I am sure that the casing may not work on the first try, as I don't currently have the assembled PCB on hand. I will be able to do some minor tweaks once the PCB is up and running. Finally, instead of making use of the *QMK* firmware, I decided to write my own in C++ using the Arduino framework. I was able to compile it in the *PlatformIO* environment and export the .uf2 file, but I don't know if it works on the real hardware yet. This is also subject to minor changes.

I started working on this project in mid-December last year, but I was busy with school most of the time and almost forgot about it. This week, during March Break, I was able to pick it up and give it a finishing touch, and I hope the final product will work fine!

Honestly speaking, I think I am also much more confident with projects like this after I successfully completed the final performance task for my Computer Engineering course at school this January. For that project, called *Funnicator*, I designed a pair of text communication devices along with my partner, which use an ESP32-S3 microcontroller and sit in a custom 3D-printed case. Our teacher liked them. It was my first time designing 3D-printed items, and it helped me a lot when designing the case for my keypad, which is even more complicated.

![](./Assets/Funnicator.JPEG)

## Schematics and PCB Design

As previously stated, I designed my PCB in EasyEDA. It is 10 cm by 10 cm exactly (this is somewhat important for minimizing the cost of the PCB, somehow), and I managed to fit a full-size arrangement of mechanical switches for a numpad. The keyboard matrix comes in a 5-row by 4-column arrangement, and takes up 9 GPIOs on the RP2040 microcontroller. Each individual switch is paired with a 1N4148 diode, to prevent current from flowing back and enable multiple keypresses at once. I used one extra GPIO for the 4 SK6812 LEDs forming a strip, and there is one vacant GPIO left. I haven't come up with a good idea for its purpose so far, but I may be able to make use of it in a later revision for a buzzer or a rotary encoder. Oh, of course, the bottom side of the board is filled with my favorite anime characters. 'Cuz why not?

![](./Schematic/Schematic_Hackpad_numeric_keypad.png)

![](./PCB/3D_PCB_top_Hackpad_numeric_keypad.png)

![](./PCB/3D_PCB_bottom_Hackpad_numeric_keypad.png)

(clockwise: Klee, Collei, Keqing, Yoimiya, and Nahida)

## Parts Required

1 x Seeed Studio XIAO RP2040 Microcontroller

4 x SK6812 Color LED

17 x Standard Mechanical Key Switches

17 x 1N4148W Diode

## Case Design

The case design is rather primitive, as I will definitely make some further tweaks once the PCB assembly is ready.  My idea for the casing is that it covers only the PCB and allows a small portion of the switch bodies to protrude above the case. This design can be found in many commercially available mechanical keyboards.

![](./CAD/HackPad - Top Casing.png)

![](./CAD/HackPad - Bottom Casing.png)

(note the cutout allowing the USB cable to plug into the microcontroller)

## Firmware

The firmware is written using the PlatformIO Arduino framework in C++. This can be changed at any time to fix bugs or add new features.

Current features of the firmware:

1. Registers the keypad as a standard USB HID device and sends inputs accordingly.

2. The NumLock key switches between number and function keys.

3. One of the LEDs in the strip indicates the current NumLock state, while the others flash on input.

4. The LED strip displays a breathing effect when the keypad is idle.

![](./Assets/Souce_code_demo.png)

## Final Words

This project is rather novel to me, as it is my first time working with a RP2040 and USB HID devices. But it turned out easier than I originally thought it would be, and I hope it will work fine once everything is put together in the end!
