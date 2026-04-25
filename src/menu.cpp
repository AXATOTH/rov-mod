#include "menu.h"
#include "hacks.h"
#include "libs/imgui/imgui.h"
#include <android/log.h>

#define LOG_TAG "ModMenu"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

// ROV Theme Colors
namespace Colors {
    static const ImVec4 Background    = ImVec4(0.08f, 0.08f, 0.12f, 0.95f);
    static const ImVec4 HeaderBg      = ImVec4(0.15f, 0.10f, 0.20f, 1.00f);
    static const ImVec4 Accent        = ImVec4(0.60f, 0.20f, 0.80f, 1.00f);
    static const ImVec4 AccentHover   = ImVec4(0.70f, 0.30f, 0.90f, 1.00f);
    static const ImVec4 TextPrimary   = ImVec4(0.90f, 0.90f, 0.95f, 1.00f);
    static const ImVec4 TextSecondary = ImVec4(0.60f, 0.60f, 0.65f, 1.00f);
    static const ImVec4 Danger        = ImVec4(0.90f, 0.20f, 0.20f, 1.00f);
    static const ImVec4 Success       = ImVec4(0.20f, 0.80f, 0.40f, 1.00f);
    static const ImVec4 Warning       = ImVec4(0.90f, 0.70f, 0.10f, 1.00f);
    static const ImVec4 EnemyRed      = ImVec4(0.95f, 0.25f, 0.25f, 1.00f);
    static const ImVec4 AllyBlue      = ImVec4(0.25f, 0.55f, 0.95f, 1.00f);
}

void applyCustomStyle() {
    ImGuiStyle& style = ImGui::GetStyle();
    
    // Rounding
    style.WindowRounding   = 12.0f;
    style.ChildRounding    = 8.0f;
    style.FrameRounding    = 8.0f;
    style.PopupRounding    = 8.0f;
    style.ScrollbarRounding= 8.0f;
    style.GrabRounding     = 6.0f;
    style.TabRounding      = 6.0f;
    
    // Sizes
    style.WindowPadding    = ImVec2(15, 15);
    style.FramePadding     = ImVec2(10, 8);
    style.ItemSpacing      = ImVec2(10, 8);
    style.ItemInnerSpacing = ImVec2(6, 4);
    style.ScrollbarSize    = 14.0f;
    style.GrabMinSize      = 12.0f;
    
    // Borders
    style.WindowBorderSize = 0.0f;
    style.ChildBorderSize  = 0.0f;
    style.FrameBorderSize  = 0.0f;
    style.PopupBorderSize  = 0.0f;
    
    // Colors
    ImVec4* c = style.Colors;
    
    // Backgrounds
    c[ImGuiCol_WindowBg]           = Colors::Background;
    c[ImGuiCol_ChildBg]            = ImVec4(0.10f, 0.10f, 0.14f, 0.60f);
    c[ImGuiCol_PopupBg]            = ImVec4(0.10f, 0.10f, 0.14f, 0.96f);
    
    // Title
    c[ImGuiCol_TitleBg]            = Colors::HeaderBg;
    c[ImGuiCol_TitleBgActive]      = Colors::HeaderBg;
    c[ImGuiCol_TitleBgCollapsed]   = ImVec4(0.10f, 0.08f, 0.14f, 0.60f);
    
    // Frames
    c[ImGuiCol_FrameBg]            = ImVec4(0.14f, 0.14f, 0.18f, 1.00f);
    c[ImGuiCol_FrameBgHovered]     = ImVec4(0.20f, 0.20f, 0.26f, 1.00f);
    c[ImGuiCol_FrameBgActive]      = ImVec4(0.24f, 0.24f, 0.30f, 1.00f);
    
    // Checkmarks & Sliders
    c[ImGuiCol_CheckMark]          = Colors::Accent;
    c[ImGuiCol_SliderGrab]         = Colors::Accent;
    c[ImGuiCol_SliderGrabActive]   = Colors::AccentHover;
    
    // Buttons
    c[ImGuiCol_Button]             = ImVec4(0.20f, 0.15f, 0.30f, 1.00f);
    c[ImGuiCol_ButtonHovered]      = Colors::Accent;
    c[ImGuiCol_ButtonActive]       = ImVec4(0.50f, 0.18f, 0.70f, 1.00f);
    
    // Headers
    c[ImGuiCol_Header]             = ImVec4(0.20f, 0.15f, 0.30f, 0.60f);
    c[ImGuiCol_HeaderHovered]      = Colors::Accent;
    c[ImGuiCol_HeaderActive]       = ImVec4(0.50f, 0.18f, 0.70f, 1.00f);
    
    // Separator
    c[ImGuiCol_Separator]          = ImVec4(0.25f, 0.25f, 0.30f, 0.60f);
    c[ImGuiCol_SeparatorHovered]   = Colors::Accent;
    c[ImGuiCol_SeparatorActive]    = Colors::AccentHover;
    
    // Tabs
    c[ImGuiCol_Tab]                = ImVec4(0.14f, 0.14f, 0.18f, 0.85f);
    c[ImGuiCol_TabHovered]         = Colors::Accent;
    c[ImGuiCol_TabActive]        = ImVec4(0.25f, 0.15f, 0.35f, 1.00f);
    c[ImGuiCol_TabUnfocused]          = ImVec4(0.12f, 0.12f, 0.16f, 0.85f);
    c[ImGuiCol_TabUnfocusedActive]  = ImVec4(0.20f, 0.12f, 0.28f, 1.00f);
    
    // Scrollbar
    c[ImGuiCol_ScrollbarBg]        = ImVec4(0.08f, 0.08f, 0.10f, 0.60f);
    c[ImGuiCol_ScrollbarGrab]      = ImVec4(0.25f, 0.25f, 0.30f, 0.80f);
    c[ImGuiCol_ScrollbarGrabHovered]= ImVec4(0.30f, 0.30f, 0.36f, 0.80f);
    c[ImGuiCol_ScrollbarGrabActive]= Colors::Accent;
    
    // Text
    c[ImGuiCol_Text]               = Colors::TextPrimary;
    c[ImGuiCol_TextDisabled]       = Colors::TextSecondary;
}

void ModMenu::init() {
    if (initialized) return;
    
    LOGI("Initializing Mod Menu...");
    applyCustomStyle();
    initialized = true;
    LOGI("Mod Menu initialized!");
}

void ModMenu::show()  { visible = true;  }
void ModMenu::hide()  { visible = false; }
void ModMenu::toggle() {
    visible = !visible;
    LOGI("Menu %s", visible ? "SHOWN" : "HIDDEN");
}
bool ModMenu::isVisible() const { return visible; }

void ModMenu::render() {
        // ---- ส่วนของปุ่มลอย (Floating Button) ----
        ImGui::SetNextWindowPos(float_btn.pos, ImGuiCond_Always);
        ImGui::Begin("FloatMenu", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing);

        if (ImGui::Button("MOD", ImVec2(60, 60))) {
            toggle();
        }

        if (ImGui::IsItemActive()) {
            ImGuiIO& io = ImGui::GetIO();
            float_btn.pos.x += io.MouseDelta.x;
            float_btn.pos.y += io.MouseDelta.y;
        }
        ImGui::End();

        // ---- ส่วนของเมนูหลัก ----
        if (!visible || !initialized) return;

        ImGuiIO& io = ImGui::GetIO();
        float sw = io.DisplaySize.x;
        float sh = io.DisplaySize.y;

        ImGui::SetNextWindowPos(ImVec2((sw - menu_width) / 2, (sh - menu_height) / 2), ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2(menu_width, menu_height), ImGuiCond_Once);

        ImGui::Begin("ROV Minimap ESP", &visible,
            ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar);

        ImGui::PushStyleColor(ImGuiCol_Text, Colors::Accent);
        ImGui::SetWindowFontScale(1.4f);
        ImGui::Text("⚔ ROV Minimap ESP");
        ImGui::SetWindowFontScale(1.0f);
        ImGui::PopStyleColor();

        ImGui::SameLine(menu_width - 100);
        auto& hacks = Hacks::getInstance();
        if (hacks.settings.enabled) {
            ImGui::TextColored(Colors::Success, "● ACTIVE");
        } else {
            ImGui::TextColored(Colors::Danger, "○ OFF");
        }

        ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

        if (ImGui::BeginTabBar("##MainTabs", ImGuiTabBarFlags_FittingPolicyResizeDown)) {
            if (ImGui::BeginTabItem("🎯 ESP")) { ui_state.current_tab = 0; ImGui::EndTabItem(); }
            if (ImGui::BeginTabItem("⚙ Settings")) { ui_state.current_tab = 1; ImGui::EndTabItem(); }
            if (ImGui::BeginTabItem("ℹ Info")) { ui_state.current_tab = 2; ImGui::EndTabItem(); }
            ImGui::EndTabBar();
        }

        ImGui::Spacing();
        switch (ui_state.current_tab) {
            case 0: renderESPTab(); break;
            case 1: renderSettingsTab(); break;
            case 2: renderInfoTab(); break;
        }

        ImGui::Spacing();
        ImGui::Separator();
        if (ImGui::Button("Hide Menu", ImVec2(-1, 0))) {
            visible = false;
        }

        ImGui::End();
}

void ModMenu::renderESPTab() {
    auto& hacks = Hacks::getInstance();
    
    // Master toggle
    ImGui::PushStyleColor(ImGuiCol_CheckMark, Colors::Success);
    if (ImGui::Checkbox("Enable ESP", &ui_state.esp_enabled)) {
        hacks.settings.enabled = ui_state.esp_enabled;
    }
    ImGui::PopStyleColor();
    
    ImGui::Spacing();
    ImGui::SeparatorText("Minimap Features");
    ImGui::Spacing();
    
    // Main feature
    if (ImGui::Checkbox("Show Enemies on Minimap", &ui_state.show_enemy_on_minimap)) {
        hacks.settings.show_enemies_on_minimap = ui_state.show_enemy_on_minimap;
    }
    ImGui::SameLine();
    ImGui::TextColored(Colors::Warning, "★");
    
    ImGui::Spacing();
    
    // Additional options
    if (ImGui::Checkbox("Show Enemy Health", &ui_state.show_enemy_health)) {
        hacks.settings.show_enemy_health = ui_state.show_enemy_health;
    }
    
    if (ImGui::Checkbox("Show Enemy Names", &ui_state.show_enemy_names)) {
        hacks.settings.show_enemy_names = ui_state.show_enemy_names;
    }
    
    if (ImGui::Checkbox("Direction Indicator", &ui_state.show_enemy_direction)) {
        hacks.settings.show_enemy_direction = ui_state.show_enemy_direction;
    }
    
    ImGui::Spacing();
    ImGui::SeparatorText("Enemy List");
    ImGui::Spacing();
    
    // Enemy count
    int enemy_count = hacks.getEnemyCount();
    ImGui::TextColored(Colors::TextSecondary, "Enemies detected: ");
    ImGui::SameLine();
    ImGui::TextColored(Colors::EnemyRed, "%d", enemy_count);
    
    // Enemy list
    auto enemies = hacks.getEnemyPlayers();
    
    if (enemies.empty()) {
        ImGui::Spacing();
        ImGui::TextColored(Colors::TextSecondary, "  No enemies detected");
        ImGui::TextColored(Colors::TextSecondary, "  Join a match to see enemies");
    } else {
        ImGui::BeginChild("##EnemyList", ImVec2(0, 140), true, ImGuiWindowFlags_HorizontalScrollbar);
        
        int idx = 0;
        for (const auto& enemy : enemies) {
            ImGui::PushID(idx++);
            
            // Status icon
            const char* icon = enemy.is_alive ? "●" : "○";
            ImVec4 color = enemy.is_alive ? Colors::EnemyRed : Colors::TextSecondary;
            
            // Hero name
            std::string name = enemy.hero_name.empty() ? "Unknown Hero" : enemy.hero_name;
            ImGui::TextColored(color, "%s %s", icon, name.c_str());
            
            // Health bar
            if (enemy.is_alive && ui_state.show_enemy_health && enemy.max_health > 0) {
                ImGui::SameLine();
                float hp_pct = enemy.health / enemy.max_health;
                ImGui::TextColored(Colors::TextSecondary, "(%.0f%%)", hp_pct * 100.0f);
            }
            
            // Position (debug)
            ImGui::SameLine();
            ImGui::TextColored(Colors::TextSecondary, "[%d]", enemy.team_id);
            
            ImGui::PopID();
        }
        
        ImGui::EndChild();
    }
}

void ModMenu::renderSettingsTab() {
    auto& hacks = Hacks::getInstance();
    
    ImGui::SeparatorText("Minimap Scale");
    ImGui::Spacing();
    
    ImGui::SetNextItemWidth(220);
    if (ImGui::SliderFloat("##Scale", &ui_state.minimap_scale, 0.5f, 2.0f, "Scale: %.2f")) {
        hacks.settings.minimap_scale = ui_state.minimap_scale;
    }
    ImGui::SameLine();
    if (ImGui::Button("Reset##Scale")) {
        ui_state.minimap_scale = 1.0f;
        hacks.settings.minimap_scale = 1.0f;
    }
    
    ImGui::Spacing();
    ImGui::SeparatorText("Enemy Marker Color");
    ImGui::Spacing();
    
    ImGui::RadioButton("Red", &ui_state.marker_color, 0); ImGui::SameLine();
    ImGui::RadioButton("Orange", &ui_state.marker_color, 1); ImGui::SameLine();
    ImGui::RadioButton("Yellow", &ui_state.marker_color, 2);
    
    ImGui::Spacing();
    ImGui::SeparatorText("Update Frequency");
    ImGui::Spacing();
    
    ImGui::RadioButton("Low (5 FPS)", &ui_state.update_freq, 0); ImGui::SameLine();
    ImGui::RadioButton("Med (10 FPS)", &ui_state.update_freq, 1);
    ImGui::RadioButton("High (20 FPS)", &ui_state.update_freq, 2);
    
    ImGui::Spacing();
    ImGui::SeparatorText("Danger Zone");
    ImGui::Spacing();
    
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.60f, 0.15f, 0.15f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.80f, 0.20f, 0.20f, 1.0f));
    if (ImGui::Button("Reset All Settings", ImVec2(200, 36))) {
        ui_state = UIState{};
        hacks.settings = Hacks::Settings{};
        hacks.settings.enabled = true;
    }
    ImGui::PopStyleColor(2);
}

void ModMenu::renderInfoTab() {
    ImGui::SeparatorText("About");
    ImGui::Spacing();
    
    ImGui::TextColored(Colors::Accent, "⚔ ROV Minimap ESP v1.0.0");
    ImGui::Spacing();
    ImGui::TextColored(Colors::TextSecondary, "Enemy Vision Hack for Arena of Valor");
    
    ImGui::Spacing();
    ImGui::SeparatorText("Target");
    ImGui::Spacing();
    
    ImGui::Text("Package: ");
    ImGui::SameLine();
    ImGui::TextColored(Colors::Accent, "com.garena.game.kgth");
    
    ImGui::Spacing();
    ImGui::SeparatorText("Features");
    ImGui::Spacing();
    
    ImGui::BulletText("Show enemy players on minimap");
    ImGui::BulletText("Bypass fog of war visibility");
    ImGui::BulletText("Real-time enemy position tracking");
    ImGui::BulletText("Enemy health status display");
    
    ImGui::Spacing();
    ImGui::SeparatorText("Status");
    ImGui::Spacing();
    
    auto& hacks = Hacks::getInstance();
    
    if (hacks.isGameRunning()) {
        ImGui::TextColored(Colors::Success, "● Game is running");
    } else {
        ImGui::TextColored(Colors::Danger, "○ Game not detected");
    }
    
    ImGui::Text("Local Team ID: %d", hacks.getLocalTeamId());
    ImGui::Text("Enemies Tracked: %d", hacks.getEnemyCount());
    
    ImGui::Spacing();
    ImGui::SeparatorText("Controls");
    ImGui::Spacing();
    
    ImGui::TextColored(Colors::TextSecondary, "3-finger tap: Toggle this menu");
    ImGui::TextColored(Colors::TextSecondary, "Back button: Close menu");
    
    ImGui::Spacing();
    ImGui::SeparatorText("Disclaimer");
    ImGui::Spacing();
    
    ImGui::TextColored(Colors::Warning, "⚠ Use at your own risk!");
    ImGui::TextWrapped("This mod violates game Terms of Service and may result in permanent account ban. Use only on test/private accounts.");
}
