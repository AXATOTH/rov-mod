#pragma once

    #include "hacks.h"
    #include "imgui.h"

    class ModMenu {
    public:
        static ModMenu& getInstance() {
            static ModMenu instance;
            return instance;
        }

        void init();
        void show();
        void hide();
        void toggle();
        bool isVisible() const;
        void render();

        float getWidth() const { return menu_width; }
        float getHeight() const { return menu_height; }

        struct FloatingButtonState {
            ImVec2 pos = ImVec2(50.0f, 50.0f);
        };
        FloatingButtonState float_btn;

    private:
        ModMenu() = default;
        ~ModMenu() = default;
        ModMenu(const ModMenu&) = delete;
        ModMenu& operator=(const ModMenu&) = delete;

        bool visible = false;
        bool initialized = false;

        float menu_width = 380.0f;
        float menu_height = 520.0f;

        void renderESPTab();
        void renderSettingsTab();
        void renderInfoTab();

        struct UIState {
            bool esp_enabled = true;
            bool show_enemy_on_minimap = true;
            bool show_enemy_health = true;
            bool show_enemy_names = true;
            bool show_enemy_direction = false;
            float minimap_scale = 1.0f;
            int current_tab = 0;
            int marker_color = 0;
            int update_freq = 1;
        };
        UIState ui_state;
    };