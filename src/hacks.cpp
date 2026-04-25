#include "hacks.h"
#include <thread>
#include <chrono>
#include <cmath>
#include <dobby.h>
#include <thread>

// Dobby hook helper
template<typename T>
bool do_hook(void* target, void* hook_fn, T* original) {
    if (!target) return false;
    int result = DobbyHook(target, hook_fn, (void**)original);
    if (result == 0) {
        LOGI("Hook installed at %p", target);
        return true;
    }
    LOGE("Hook failed at %p (error: %d)", target, result);
    return false;
}

void Hacks::init() {
    if (initialized) return;
    
    LOGI("=== Initializing Hacks ===");
    
    // Start initialization in background
    std::thread([this]() {
        // Wait for game to fully load
        LOGI("Waiting for game classes...");
        
        int retries = 0;
        while (retries < 60) {
            // Check if IL2CPP is ready
            if (!il2cpp::isInitialized()) {
                il2cpp::init();
            }
            
            // Try to find game classes
            findGameClasses();
            
            if (player_class) {
                LOGI("Game classes found!");
                break;
            }
            
            std::this_thread::sleep_for(std::chrono::seconds(1));
            retries++;
            LOGD("Retry %d/60...", retries);
        }
        
        if (!player_class) {
            LOGE("Failed to find game classes after 60 seconds");
            return;
        }
        
        // Hook game functions
        hookGameFunctions();
        
        initialized = true;
        settings.enabled = true;
        
        // Start update loop
        running = true;
        update_thread = std::thread(&Hacks::updateLoop, this);
        
        LOGI("=== Hacks initialized successfully! ===");
    }).detach();
}

void Hacks::findGameClasses() {
    // Common class names for MOBA games
    const char* player_classes[] = {
        "PlayerController", "HeroController", "UnitController",
        "CharacterController", "PlayerEntity", "HeroEntity",
        "BattlePlayer", "GamePlayer", nullptr
    };
    
    for (int i = 0; player_classes[i]; i++) {
        player_class = il2cpp::get_class("Assembly-CSharp", "", player_classes[i]);
        if (player_class) {
            LOGI("Found player class: %s", player_classes[i]);
            break;
        }
    }
    
    if (!player_class) {
        // Try searching in other assemblies
        const char* assemblies[] = {
            "Assembly-CSharp", "Game.Runtime", "Battle", nullptr
        };
        
        for (int a = 0; assemblies[a]; a++) {
            for (int i = 0; player_classes[i]; i++) {
                player_class = il2cpp::get_class(assemblies[a], "", player_classes[i]);
                if (player_class) {
                    LOGI("Found player class in %s: %s", assemblies[a], player_classes[i]);
                    break;
                }
            }
            if (player_class) break;
        }
    }
    
    if (!player_class) return;
    
    // Find methods with various common names
    const char* pos_methods[] = {"get_Position", "get_WorldPosition", "GetWorldPosition", "get_transform_position", nullptr};
    for (int i = 0; pos_methods[i]; i++) {
        get_position_method = il2cpp::get_method(player_class, pos_methods[i]);
        if (get_position_method) {
            LOGI("Found position method: %s", pos_methods[i]);
            break;
        }
    }
    
    const char* team_methods[] = {"get_TeamId", "get_Team", "get_Camp", "GetTeam", "get_Faction", nullptr};
    for (int i = 0; team_methods[i]; i++) {
        get_team_method = il2cpp::get_method(player_class, team_methods[i]);
        if (get_team_method) {
            LOGI("Found team method: %s", team_methods[i]);
            break;
        }
    }
    
    const char* alive_methods[] = {"IsAlive", "get_IsAlive", "get_Alive", "IsDead", nullptr};
    for (int i = 0; alive_methods[i]; i++) {
        is_alive_method = il2cpp::get_method(player_class, alive_methods[i]);
        if (is_alive_method) {
            LOGI("Found alive method: %s", alive_methods[i]);
            break;
        }
    }
    
    const char* hp_methods[] = {"get_CurrentHP", "get_Hp", "get_Health", "GetCurrentHealth", nullptr};
    for (int i = 0; hp_methods[i]; i++) {
        get_health_method = il2cpp::get_method(player_class, hp_methods[i]);
        if (get_health_method) break;
    }
    
    const char* max_hp_methods[] = {"get_MaxHP", "get_MaxHp", "get_MaxHealth", nullptr};
    for (int i = 0; max_hp_methods[i]; i++) {
        get_max_health_method = il2cpp::get_method(player_class, max_hp_methods[i]);
        if (get_max_health_method) break;
    }
    
    // Find get all players method
    Il2CppClass* gm_class = il2cpp::get_class("Assembly-CSharp", "", "GameManager");
    if (!gm_class) gm_class = il2cpp::get_class("Assembly-CSharp", "", "GameplayManager");
    
    if (gm_class) {
        const char* player_list_methods[] = {"GetAllPlayers", "get_AllPlayers", "GetPlayerList", "GetPlayers", nullptr};
        for (int i = 0; player_list_methods[i]; i++) {
            get_all_players_method = il2cpp::get_method(gm_class, player_list_methods[i]);
            if (get_all_players_method) {
                LOGI("Found GetAllPlayers: %s", player_list_methods[i]);
                break;
            }
        }
    }
}

void Hacks::hookGameFunctions() {
    LOGI("Hooking game functions...");
    
    // Hook minimap update
    const char* minimap_classes[] = {
        "MinimapController", "MinimapManager", "UIMinimap", "MiniMapView", nullptr
    };
    
    for (int i = 0; minimap_classes[i]; i++) {
        Il2CppClass* klass = il2cpp::get_class("Assembly-CSharp", "", minimap_classes[i]);
        if (!klass) continue;
        
        // Get update method
        const char* update_methods[] = {"Update", "UpdateMinimap", "RefreshMinimap", "LateUpdate", nullptr};
        for (int j = 0; update_methods[j]; j++) {
            MethodInfo* method = il2cpp::get_method(klass, update_methods[j]);
            if (method) {
                // Get method pointer from MethodInfo
                void** method_ptr = reinterpret_cast<void**>(&((uint64_t*)method)[0]); // methodPointer offset
                // Actual offset depends on IL2CPP version, this is simplified
                LOGI("Would hook %s.%s", minimap_classes[i], update_methods[j]);
                break;
            }
        }
        break;
    }
    
    // Hook visibility/fog system
    const char* visibility_classes[] = {
        "FogOfWarSystem", "VisibilityManager", "VisionSystem",
        "FOWManager", "FogSystem", "VisionController", nullptr
    };
    
    for (int i = 0; visibility_classes[i]; i++) {
        Il2CppClass* klass = il2cpp::get_class("Assembly-CSharp", "", visibility_classes[i]);
        if (!klass) continue;
        
        const char* methods[] = {"IsUnitVisible", "IsVisible", "GetVisibility", "CanSeeUnit", nullptr};
        for (int j = 0; methods[j]; j++) {
            MethodInfo* method = il2cpp::get_method(klass, methods[j]);
            if (method) {
                LOGI("Found visibility method: %s.%s", visibility_classes[i], methods[j]);
                // Hook would go here with Dobby
                break;
            }
        }
        break;
    }
    
    LOGI("Hooking complete");
}

void Hacks::updateLoop() {
    LOGI("Update loop started");
    
    while (running) {
        if (settings.enabled && settings.show_enemies_on_minimap) {
            scanAllPlayers();
            updateEnemyPositions();
        }
        
        // 10 FPS update rate
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    LOGI("Update loop ended");
}

void Hacks::update() {
    if (!initialized || !settings.enabled) return;
    scanAllPlayers();
}

void Hacks::scanAllPlayers() {
    std::lock_guard<std::mutex> lock(players_mutex);
    cached_enemies.clear();
    cached_allies.clear();
    
    // Get local player
    Il2CppObject* local_player = il2cpp::get_local_player();
    if (!local_player) return;
    
    // Get local team
    if (get_team_method) {
        typedef int (*GetTeamFunc)(Il2CppObject*, MethodInfo*);
        local_team_id = reinterpret_cast<GetTeamFunc>(get_team_method)(local_player, nullptr);
    }
    
    // Get game manager
    Il2CppObject* game_mgr = il2cpp::get_game_manager();
    if (!game_mgr) return;
    
    // Get all players
    if (!get_all_players_method) return;
    
    typedef Il2CppArray* (*GetPlayersFunc)(Il2CppObject*, MethodInfo*);
    Il2CppArray* players = reinterpret_cast<GetPlayersFunc>(get_all_players_method)(game_mgr, nullptr);
    if (!players) return;
    
    size_t count = il2cpp::array_length(players);
    LOGD("Scanning %zu players", count);
    
    // Iterate all players
    for (size_t i = 0; i < count; i++) {
        Il2CppObject* player = *reinterpret_cast<Il2CppObject**>(
            il2cpp::array_addr(players, sizeof(void*), i)
        );
        
        if (!player || player == local_player) continue;
        
        PlayerInfo info{};
        info.player_object = player;
        
        // Get team
        if (get_team_method) {
            typedef int (*GetTeamFunc)(Il2CppObject*, MethodInfo*);
            info.team_id = reinterpret_cast<GetTeamFunc>(get_team_method)(player, nullptr);
            info.is_enemy = (info.team_id != local_team_id);
        }
        
        // Get position
        if (get_position_method) {
            typedef Vector3 (*GetPosFunc)(Il2CppObject*, MethodInfo*);
            info.world_position = reinterpret_cast<GetPosFunc>(get_position_method)(player, nullptr);
            info.minimap_position = worldToMinimap(info.world_position);
        }
        
        // Get alive status
        if (is_alive_method) {
            typedef bool (*IsAliveFunc)(Il2CppObject*, MethodInfo*);
            info.is_alive = reinterpret_cast<IsAliveFunc>(is_alive_method)(player, nullptr);
        } else {
            info.is_alive = true; // Assume alive if method not found
        }
        
        // Get health
        if (get_health_method) {
            typedef float (*GetHPFunc)(Il2CppObject*, MethodInfo*);
            info.health = reinterpret_cast<GetHPFunc>(get_health_method)(player, nullptr);
        }
        
        if (get_max_health_method) {
            typedef float (*GetMaxHPFunc)(Il2CppObject*, MethodInfo*);
            info.max_health = reinterpret_cast<GetMaxHPFunc>(get_max_health_method)(player, nullptr);
        }
        
        // Categorize
        if (info.is_enemy) {
            cached_enemies.push_back(info);
        } else {
            cached_allies.push_back(info);
        }
    }
    
    LOGD("Found %zu enemies, %zu allies", cached_enemies.size(), cached_allies.size());
}

void Hacks::updateEnemyPositions() {
    std::lock_guard<std::mutex> lock(players_mutex);
    
    for (auto& enemy : cached_enemies) {
        if (!enemy.is_alive || !enemy.player_object || !get_position_method) continue;
        
        typedef Vector3 (*GetPosFunc)(Il2CppObject*, MethodInfo*);
        enemy.world_position = reinterpret_cast<GetPosFunc>(get_position_method)(enemy.player_object, nullptr);
        enemy.minimap_position = worldToMinimap(enemy.world_position);
    }
}

void Hacks::revealEnemiesOnMinimap() {
    // This is called from hook callbacks to force-show enemies
    for (const auto& enemy : cached_enemies) {
        if (!enemy.is_alive) continue;
        
        // The actual revealing happens by modifying visibility/fog system
        // or by directly manipulating minimap markers
        LOGD("Enemy at: (%.1f, %.1f, %.1f)", 
             enemy.world_position.x, enemy.world_position.y, enemy.world_position.z);
    }
}

Vector3 Hacks::worldToMinimap(Vector3 world_pos) {
    // ROV map coordinate transformation
    // Adjust these values based on actual game map size
    float map_size = 120.0f;  // Approximate MOBA map size
    float half_map = map_size / 2.0f;
    
    Vector3 minimap;
    minimap.x = (world_pos.x + half_map) / map_size;
    minimap.y = 0.0f;
    minimap.z = (world_pos.z + half_map) / map_size;
    
    // Clamp to 0-1 range
    minimap.x = fmaxf(0.0f, fminf(1.0f, minimap.x));
    minimap.z = fmaxf(0.0f, fminf(1.0f, minimap.z));
    
    // Apply scale
    minimap.x *= settings.minimap_scale;
    minimap.z *= settings.minimap_scale;
    
    return minimap;
}

bool Hacks::isEnemyTeam(int team_id) {
    return team_id != local_team_id && team_id > 0;
}

std::vector<PlayerInfo> Hacks::getEnemyPlayers() {
    std::lock_guard<std::mutex> lock(players_mutex);
    return cached_enemies;
}

std::vector<PlayerInfo> Hacks::getAllPlayers() {
    std::lock_guard<std::mutex> lock(players_mutex);
    std::vector<PlayerInfo> all;
    all.insert(all.end(), cached_enemies.begin(), cached_enemies.end());
    all.insert(all.end(), cached_allies.begin(), cached_allies.end());
    return all;
}

int Hacks::getLocalTeamId() {
    return local_team_id;
}

bool Hacks::isGameRunning() {
    return initialized && il2cpp::get_local_player() != nullptr;
}

int Hacks::getEnemyCount() {
    std::lock_guard<std::mutex> lock(players_mutex);
    return (int)cached_enemies.size();
}

void Hacks::cleanup() {
    running = false;
    
    if (update_thread.joinable()) {
        update_thread.join();
    }
    
    std::lock_guard<std::mutex> lock(players_mutex);
    cached_enemies.clear();
    cached_allies.clear();
    
    initialized = false;
}

Hacks::~Hacks() {
    cleanup();
}

// Static hook callbacks
void Hacks::onMinimapUpdate(Il2CppObject* minimap, MethodInfo* method) {
    auto& hacks = Hacks::getInstance();
    
    // Call original
    if (hacks.orig_minimap_update) {
        typedef void (*OrigFunc)(Il2CppObject*, MethodInfo*);
        reinterpret_cast<OrigFunc>(hacks.orig_minimap_update)(minimap, method);
    }
    
    // Inject our ESP
    if (hacks.settings.enabled) {
        hacks.revealEnemiesOnMinimap();
    }
}

bool Hacks::onVisibilityCheck(Il2CppObject* unit, MethodInfo* method) {
    auto& hacks = Hacks::getInstance();
    
    // Force visible for enemies when ESP is on
    if (hacks.settings.enabled && hacks.settings.show_enemies_on_minimap) {
        // Check if enemy
        if (hacks.get_team_method && hacks.player_class) {
            typedef int (*GetTeamFunc)(Il2CppObject*, MethodInfo*);
            int team = reinterpret_cast<GetTeamFunc>(hacks.get_team_method)(unit, nullptr);
            if (hacks.isEnemyTeam(team)) {
                return true;  // Force visible
            }
        }
    }
    
    // Call original
    if (hacks.orig_visibility_check) {
        typedef bool (*OrigFunc)(Il2CppObject*, MethodInfo*);
        return reinterpret_cast<OrigFunc>(hacks.orig_visibility_check)(unit, method);
    }
    
    return true;
}

int Hacks::onFogCheck(Il2CppObject* unit, MethodInfo* method) {
    auto& hacks = Hacks::getInstance();
    
    // Return "visible" for enemies
    if (hacks.settings.enabled && hacks.settings.show_enemies_on_minimap) {
        if (hacks.get_team_method) {
            typedef int (*GetTeamFunc)(Il2CppObject*, MethodInfo*);
            int team = reinterpret_cast<GetTeamFunc>(hacks.get_team_method)(unit, nullptr);
            if (hacks.isEnemyTeam(team)) {
                return 2;  // Fully visible
            }
        }
    }
    
    // Call original
    if (hacks.orig_fog_check) {
        typedef int (*OrigFunc)(Il2CppObject*, MethodInfo*);
        return reinterpret_cast<OrigFunc>(hacks.orig_fog_check)(unit, method);
    }
    
    return 0;
}
