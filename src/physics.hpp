#pragma once

#include <cmath>

// Tuning constants for the game physics, all in pixels and seconds.
// tools/gen_level.py mirrors the jump constants to check that levels are completable,
// so keep the two in sync when changing them.

namespace nomi::phys
{

constexpr float TileSize        = 64.f;

constexpr float Gravity         = 3000.f;  // px/s^2
constexpr float MaxFallSpeed    = 1400.f;  // px/s

constexpr float PlayerMaxSpeed  = 420.f;   // px/s
constexpr float PlayerAccel     = 3200.f;  // px/s^2 on the ground
constexpr float PlayerAirAccel  = 2000.f;  // px/s^2 in the air
constexpr float PlayerFriction  = 2800.f;  // px/s^2 when no key is held
constexpr float JumpHeight      = 230.f;   // px, a bit more than 3.5 tiles
constexpr float JumpCutFactor   = 0.45f;   // upward speed kept when jump is released early
constexpr float CoyoteTime      = 0.10f;   // s, can still jump just after leaving a ledge
constexpr float JumpBufferTime  = 0.12f;   // s, jump pressed just before landing still counts
constexpr float StompBounce     = 750.f;   // px/s upward after stomping an enemy
constexpr float InvulnerableTime= 2.0f;    // s after respawning

constexpr float EnemySpeed      = 90.f;    // px/s default patrol speed

// initial upward speed needed to reach the jump height : v = sqrt(2 g h)
inline float jumpVelocity( float height = JumpHeight ) { return std::sqrt( 2.f * Gravity * height ); }

// A contact counts as a stomp when the player is falling and was above the enemy's
// top edge in the previous tick (with a little tolerance).
inline bool isStomp( float playerPrevBottom, float playerVelocityY, float enemyTop )
{
    return playerVelocityY > 0.f && playerPrevBottom <= enemyTop + 12.f;
}

} // namespace nomi::phys
