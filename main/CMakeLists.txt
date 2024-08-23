set(srcs "encoder"
         "backlight"
         "lvgl_demo"
         "i2cmaster"
         "dio"
         ".")


# In order for the cases defined by `TEST_CASE` to be linked into the final elf,
# the component can be registered as WHOLE_ARCHIVE
idf_component_register(SRC_DIRS ${srcs}
                       PRIV_REQUIRES esp_lcd unity driver led_strip
                       WHOLE_ARCHIVE)

