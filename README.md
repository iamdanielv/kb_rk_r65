# RK ROYAL KLUDGE R65 - 🚧 Custom QMK firmware

I bought this keyboard from amazon:
[RK ROYAL KLUDGE R65](https://www.amazon.com/dp/B0CNP1BG1W?th=1)

It quickly became one of my favorite keyboards, it's a great size and even has a knob! ([Hipyo Tech](https://www.youtube.com/@HipyoTech) would be proud)

Similarly to [my GK61](https://github.com/iamdanielv/kb_gk61), I wanted to program it.

Getting the original source for the firmware has not been easy, but I found [@irfan](https://github.com/irfanjmdn/r65) had started the ball rolling over on Discord.

We have been working to get a working copy of the firmware that is at least feature parity to the shipped firmware. -- We are not exactly there yet, but are very close!

## Custom Keyboard Software

Since I don't have the original source, I decided to create a new folder called `iamdanielv` that could be put into the qmk repo `keyboard` directory and not mess anything up when/if the original source is added later.

Copy the folder structure from `rk\r65\iamdanielv` into the qmk repo `keyboard` directory.

## ⚠️ Disclaimer
> [!CAUTION]
> This is a custom firmware that could break your keyboard. 🛑 **Use at your own risk!** 🛑
> 
> I have tried my best to make sure everything works, but I may miss something.
>
> It works for my keyboard, but there may be revisions in the future that are not compatible. Use at your own risk!

## 🥳Thanks!

Thanks to [@irfan](https://github.com/irfanjmdn/) for getting the ball rolling on getting the PCB and source code.

Thanks to [sdk66](https://github.com/sdk66/) for providing a lot of samples and guidance.
The [Tide65 code base](https://github.com/sdk66/qmk_firmware/tree/wireless/keyboards/epomaker/tide65) was very helpful in getting a lot of things figured out.

## ⌨️ New Keymap

I've updated the **default** keymap to work with the current source. It can be used as a starting point for customization.

I created a **new keymap** `iamdanielv` that enables via, function keys, navigation, a num pad and layer for RGB / Keyboard management.

I use `6 layers` for my changes, `keyboard.json` specifies that in the following section:

```json
"dynamic_keymap": {
    "layer_count": 6
}
```

## 🖥️ Compiling

You can now use QMK builder with the following command:

```shell
qmk compile -j 0 -kb rk/r65/iamdanielv -km iamdanielv
```

> the `-j 0` uses parallel build to speed it up a bit

qmk should generate a new bin file: `rk_r65_iamdanielv_iamdanielv.bin` in your qmk_firmware folder.

For me it was: `C:\Users\<USER>\qmk_firmware\rk_r65_iamdanielv_iamdanielv.bin`

I've included a version of [my custom firmware](extras/rk_r65_iamdanielv_iamdanielv.bin) that works well for me. There you will also find the [original firmware](extras/original_rk_r65_firmware.hex) in case something goes horribly wrong.

- [Original](extras/original_rk_r65_firmware.hex)
- [Custom](extras/rk_r65_iamdanielv_iamdanielv.bin)

## 🌐 Using via

In order to use via, you may have to upload the [json specification](extras/rk_r65_via.json), it can be found under the `extras` folder.

## Bootloader

To prevent accidental KB Bootloader triggers, I have it hidden under a layer.
- To reboot and `enter bootloader mode`: push `Fn`, then `shift`, then while holding `shift` triple tap the `q` key.
- To `clear eeprom`: push `Fn`, then `shift`, then while holding `shift` triple tap the `z` key.

## 🎉 Conclusion and Next Steps

**That's it!** You now should be able to flash the custom firmware using qmk toolbox.

Let me know if you have any questions. I can be reached at [@IAmDanielV](https://twitter.com/IAmDanielV).

