#pragma once

#include "il2cpp.h"
#include <vector>
#include <mutex>
#include <atomic>

// Player info for ESP
struct PlayerInfo {
    Il2CppObject* player_object = nullptr;
    Vector3 world_position;
    Vector3 minimap_position;
    bool is_enemy = false;
    bool is_alive = false;
    bool is_visible = false;
    int team_id = 0;
    int hero_id = 0;
    std::string hero_name;
    float health = 0;
    float max_health = 100;
    float level = 1;
};

class Hacks {
public:
    static Hacks& getInstance() {
        static Hacks instance;
        return instance;
    }
    
    void init();
    void update();
    void cleanup();
    
    // Main hack functions
    void revealEnemiesOnMinimap();
    void updateEnemyPositions();
    void scanAllPlayers();
    
    // Hook callbacks
    static void onMinimapUpdate(Il2CppObject* minimap, MethodInfo* method);
    static bool onVisibilityCheck(Il2CppObject* unit, MethodInfo* method);
    static int onFogCheck(Il2CppObject* unit, MethodInfo* method);
    
    // Settings
    struct Settings {
        bool enabled = false;
        bool show_enemies_on_minimap = true;
        bool show_enemy_health = true;
        bool show_enemy_names = true;
        bool show_enemy_direction = false;
        float minimap_scale = 1.0f;
    };
    
    Settings settings;
    
    // Getters
    std::vector<PlayerInfo> getEnemyPlayers();
    std::vector<PlayerInfo> getAllPlayers();
    int getLocalTeamId();
    bool isGameRunning();
    int getEnemyCount();
    
private:
    Hacks() = default;
    ~Hacks();
    
    Hacks(const Hacks&) = delete;
    Hacks& operator=(const Hacks&) = delete;
    
    // State
    std::vector<PlayerInfo> cached_enemies;
    std::vector<PlayerInfo> cached_allies;
    std::mutex players_mutex;
    std::atomic<bool> initialized{false};
    std::atomic<bool> running{false};
    int local_team_id = -1;
    
    // Update thread
    void updateLoop();
    std::thread update_thread;
    
    // Original function pointers
    void* orig_minimap_update = nullptr;
    void* orig_visibility_check = nullptr;
    void* orig_fog_check = nullptr;
    
    // Helpers
    Vector3 worldToMinimap(Vector3 world_pos);
    bool isEnemyTeam(int team_id);
    void hookGameFunctions();
    void findGameClasses();
    
    // Cached class/method pointers
    Il2CppClass* player_class = nullptr;
    MethodInfo* get_position_method = nullptr;
    MethodInfo* get_team_method = nullptr;
    MethodInfo* is_alive_method = nullptr;
    MethodInfo* get_health_method = nullptr;
    MethodInfo* get_max_health_method = nullptr;
    MethodInfo* get_hero_data_method = nullptr;
    MethodInfo* get_hero_name_method = nullptr;
    MethodInfo* get_all_players_method = nullptr;
};
