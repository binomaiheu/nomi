#include <cmath>
#include <cstdio>

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Window/Joystick.hpp>
#include <SFML/Window/Keyboard.hpp>

#include "states.hpp"
#include "utils.hpp"
#include "version.hpp"

namespace nomi
{

namespace
{
bool isKey( const sf::Event& ev, sf::Keyboard::Key key )
{
    const auto* k = ev.getIf<sf::Event::KeyPressed>();
    return k && k->code == key;
}

bool isConfirm( const sf::Event& ev )
{
    if ( isKey( ev, sf::Keyboard::Key::Enter ) || isKey( ev, sf::Keyboard::Key::Space ) ) return true;
    const auto* b = ev.getIf<sf::Event::JoystickButtonPressed>();
    return b && ( b->button == 0 || b->button == 7 );
}

void toggleMute( Context& ctx )
{
    const bool mute = !ctx.music.isMuted();
    ctx.music.setMuted( mute );
    ctx.sounds.setMuted( mute );
}

// sky colour, a strip of the backdrop hills and a grass floor, used behind the menus
void drawScenery( sf::RenderTarget& target, Context& ctx, float scroll )
{
    const sf::Vector2f size = ctx.viewSize;
    sf::RectangleShape sky( size );
    sky.setFillColor( sf::Color( 0xc0, 0xf1, 0xef ) );
    target.draw( sky );

    const auto& backdrop = ctx.textures.load( "backdrop", dataPath( "tilesets/backdrop.png" ) );
    const float bw = float( backdrop.getSize().x );
    sf::Sprite hills( backdrop );
    const float groundY = size.y - 128.f;
    for ( float x = -std::fmod( scroll * 0.3f, bw ); x < size.x; x += bw ) {
        hills.setPosition( { x, groundY - backdrop.getSize().y + 64.f } );
        target.draw( hills );
    }

    const auto& tiles = ctx.textures.load( "tileset", dataPath( "tilesets/tileset.png" ) );
    sf::Sprite grass( tiles, sf::IntRect( { 0, 320 }, { 64, 64 } ) );
    sf::Sprite dirt( tiles, sf::IntRect( { 320, 320 }, { 64, 64 } ) );
    for ( float x = -std::fmod( scroll, 64.f ); x < size.x; x += 64.f ) {
        grass.setPosition( { x, groundY } );
        dirt.setPosition( { x, groundY + 64.f } );
        target.draw( grass );
        target.draw( dirt );
    }
}
}

Player::Input readInput()
{
    using K = sf::Keyboard::Key;
    Player::Input in;
    in.left  = sf::Keyboard::isKeyPressed( K::Left )  || sf::Keyboard::isKeyPressed( K::A );
    in.right = sf::Keyboard::isKeyPressed( K::Right ) || sf::Keyboard::isKeyPressed( K::D );
    in.jump  = sf::Keyboard::isKeyPressed( K::Space ) || sf::Keyboard::isKeyPressed( K::Up ) ||
               sf::Keyboard::isKeyPressed( K::W );

    if ( sf::Joystick::isConnected( 0 ) ) {
        const float x = sf::Joystick::getAxisPosition( 0, sf::Joystick::Axis::X );
        const float pov = sf::Joystick::getAxisPosition( 0, sf::Joystick::Axis::PovX );
        in.left  |= x < -40.f || pov < -40.f;
        in.right |= x >  40.f || pov >  40.f;
        in.jump  |= sf::Joystick::isButtonPressed( 0, 0 ) || sf::Joystick::isButtonPressed( 0, 1 );
    }
    return in;
}


// ---------------------------------------------------------------- title

TitleState::TitleState( StateStack& stack, Context& ctx )
    : State( stack, ctx )
    , mSelected( ctx.session.character )
{
    for ( const auto& c : characters() )
        ctx.textures.load( "char/" + c.sheet, dataPath( "pixmaps/" + c.sheet ) );
    mIdle.play( CharacterAnimations::get().idle );
    mWalk.play( CharacterAnimations::get().walk );
    ctx.music.play( "title.ogg", 35.f );
}

void TitleState::handleEvent( const sf::Event& ev )
{
    const std::size_t n = characters().size();
    const auto* stick = ev.getIf<sf::Event::JoystickMoved>();
    const bool left  = isKey( ev, sf::Keyboard::Key::Left )  || isKey( ev, sf::Keyboard::Key::A ) ||
                       ( stick && stick->position < -60.f && stick->axis == sf::Joystick::Axis::X );
    const bool right = isKey( ev, sf::Keyboard::Key::Right ) || isKey( ev, sf::Keyboard::Key::D ) ||
                       ( stick && stick->position >  60.f && stick->axis == sf::Joystick::Axis::X );

    if ( left )  { mSelected = ( mSelected + n - 1 ) % n; context().sounds.play( "select" ); }
    if ( right ) { mSelected = ( mSelected + 1 ) % n;     context().sounds.play( "select" ); }

    if ( isConfirm( ev ) ) {
        context().sounds.play( "start" );
        context().session.reset( mSelected );
        stack().clear();
        stack().push<PlayingState>( context().firstLevel );
    }
    if ( isKey( ev, sf::Keyboard::Key::M ) ) toggleMute( context() );
    if ( isKey( ev, sf::Keyboard::Key::Escape ) && context().window ) context().window->close();
}

void TitleState::update( sf::Time dt )
{
    mTime += dt.asSeconds();
    mIdle.update( dt );
    mWalk.update( dt );
}

void TitleState::draw( sf::RenderTarget& target )
{
    Context& ctx = context();
    const sf::Vector2f size = ctx.viewSize;
    drawScenery( target, ctx, mTime * 60.f );

    const auto& titleFont = loadFont( ctx, TitleFont );
    const auto& textFont  = loadFont( ctx, TextFont );

    sf::Text title = makeText( titleFont, "NOMI", 150, sf::Color( 255, 200, 60 ), 8.f );
    centerOrigin( title );
    title.setPosition( { size.x / 2.f, 150.f + 8.f * std::sin( mTime * 2.f ) } );
    target.draw( title );

    sf::Text sub = makeText( textFont, "Choose your hero", 56 );
    centerOrigin( sub );
    sub.setPosition( { size.x / 2.f, 290.f } );
    target.draw( sub );

    // the characters standing on the grass, the selected one walks on the spot and is bigger
    const auto& chars = characters();
    const float spacing = 200.f;
    const float x0 = size.x / 2.f - spacing * ( chars.size() - 1 ) / 2.f;
    const float floorY = size.y - 128.f;
    for ( std::size_t i = 0; i < chars.size(); ++i ) {
        const bool sel = i == mSelected;
        sf::Sprite s( ctx.textures.get( "char/" + chars[i].sheet ) );
        s.setTextureRect( sel ? mWalk.frame() : mIdle.frame() );
        s.setOrigin( { CharacterAnimations::FrameWidth / 2.f, float( CharacterAnimations::FrameHeight ) } );
        s.setScale( sel ? sf::Vector2f( 2.f, 2.f ) : sf::Vector2f( 1.4f, 1.4f ) );
        s.setPosition( { x0 + i * spacing, floorY } );
        if ( !sel ) s.setColor( sf::Color( 255, 255, 255, 170 ) );
        target.draw( s );

        sf::Text name = makeText( textFont, chars[i].name, sel ? 52 : 40,
                                  sel ? sf::Color( 255, 230, 120 ) : sf::Color( 230, 230, 230 ) );
        centerOrigin( name );
        name.setPosition( { x0 + i * spacing, floorY + 40.f } );
        target.draw( name );
    }

    sf::Text hint = makeText( textFont, "<  >  choose      ENTER  start      M  mute      ESC  quit", 36 );
    centerOrigin( hint );
    hint.setPosition( { size.x / 2.f, size.y - 30.f } );
    target.draw( hint );

    sf::Text version = makeText( textFont, "v" NOMI_VERSION, 28, sf::Color( 255, 255, 255, 200 ), 2.f );
    version.setPosition( { size.x - version.getLocalBounds().size.x - 16.f, 8.f } );
    target.draw( version );
}


// ---------------------------------------------------------------- playing

PlayingState::PlayingState( StateStack& stack, Context& ctx, std::string levelFile )
    : State( stack, ctx )
    , mLevelFile( std::move( levelFile ) )
    , mHud( ctx )
{
    const auto& c = characters()[ ctx.session.character % characters().size() ];
    mWorld = std::make_unique<World>( mLevelFile, ctx.textures, c.sheet, ctx.viewSize );
    ctx.music.play( "level.mp3", 35.f );
}

void PlayingState::handleEvent( const sf::Event& ev )
{
    const auto* b = ev.getIf<sf::Event::JoystickButtonPressed>();
    if ( isKey( ev, sf::Keyboard::Key::Escape ) || isKey( ev, sf::Keyboard::Key::P ) || ( b && b->button == 7 ) )
        stack().push<MessageState>( MessageState::Kind::Paused );
    if ( isKey( ev, sf::Keyboard::Key::M ) ) toggleMute( context() );
    if ( isKey( ev, sf::Keyboard::Key::F1 ) ) gDebugDraw = !gDebugDraw;
}

void PlayingState::update( sf::Time dt )
{
    mTime += dt.asSeconds();
    mWorld->update( dt, readInput() );
    handleWorldEvents();

    if ( mCompleteTimer >= 0.f ) {
        mCompleteTimer -= dt.asSeconds();
        if ( mCompleteTimer < 0.f ) {
            const std::string next = mWorld->level().nextLevel();
            stack().push<MessageState>( next.empty() ? MessageState::Kind::Victory : MessageState::Kind::LevelComplete, next );
        }
    }
}

void PlayingState::handleWorldEvents()
{
    Context& ctx = context();
    for ( const auto& ev : mWorld->pollEvents() )
    {
        switch ( ev.type )
        {
        case WorldEvent::Jumped:
            ctx.sounds.play( "jump" );
            break;
        case WorldEvent::CoinCollected:
            ctx.sounds.play( ctx.session.addCoin() ? "oneup" : "coin" );
            break;
        case WorldEvent::EnemyStomped:
            ctx.session.addStomp();
            ctx.sounds.play( "stomp" );
            break;
        case WorldEvent::PlayerKilled:
            ctx.music.stop();
            ctx.sounds.play( "hurt" );
            break;
        case WorldEvent::PlayerDied:
            if ( ctx.session.loseLife() ) {
                ctx.music.play( "level.mp3", 35.f );
            } else {
                stack().push<MessageState>( MessageState::Kind::GameOver );
            }
            break;
        case WorldEvent::LevelComplete:
            ctx.session.addGoal();
            ctx.music.stop();
            ctx.sounds.play( "goal" );
            mCompleteTimer = 2.0f;
            break;
        }
    }
}

void PlayingState::draw( sf::RenderTarget& target )
{
    sf::RectangleShape sky( context().viewSize );
    sky.setFillColor( mWorld->level().backgroundColor() );
    target.draw( sky );

    mWorld->draw( target );
    mHud.draw( target, context().session, mWorld->level().name(), mTime );
}


// ---------------------------------------------------------------- messages

MessageState::MessageState( StateStack& stack, Context& ctx, Kind kind, std::string nextLevel )
    : State( stack, ctx )
    , mKind( kind )
    , mNextLevel( std::move( nextLevel ) )
{
    if ( kind == Kind::GameOver ) {
        ctx.music.stop();
        ctx.sounds.play( "gameover" );
    } else if ( kind == Kind::Victory ) {
        ctx.sounds.play( "victory" );
    }
}

void MessageState::proceed()
{
    switch ( mKind )
    {
    case Kind::Paused:
        stack().pop();
        break;
    case Kind::LevelComplete:
        stack().clear();
        stack().push<PlayingState>( mNextLevel );
        break;
    case Kind::GameOver:
    case Kind::Victory:
        stack().clear();
        stack().push<TitleState>();
        break;
    }
}

void MessageState::handleEvent( const sf::Event& ev )
{
    if ( mKind == Kind::Paused ) {
        if ( isKey( ev, sf::Keyboard::Key::Q ) ) {
            stack().clear();
            stack().push<TitleState>();
            return;
        }
        if ( isKey( ev, sf::Keyboard::Key::M ) ) toggleMute( context() );
        if ( isKey( ev, sf::Keyboard::Key::Escape ) || isKey( ev, sf::Keyboard::Key::P ) || isConfirm( ev ) ) proceed();
        return;
    }
    // ignore key presses in the first moment so a held jump key doesn't skip the message
    if ( mTime > 0.8f && isConfirm( ev ) ) proceed();
}

void MessageState::update( sf::Time dt )
{
    mTime += dt.asSeconds();
    if ( mKind == Kind::LevelComplete && mTime > 4.f ) proceed();
}

void MessageState::draw( sf::RenderTarget& target )
{
    Context& ctx = context();
    const sf::Vector2f size = ctx.viewSize;

    sf::RectangleShape shade( size );
    shade.setFillColor( sf::Color( 0, 0, 30, 140 ) );
    target.draw( shade );

    std::string title, line1, line2;
    char buf[64];
    std::snprintf( buf, sizeof( buf ), "Score %d", ctx.session.score );
    switch ( mKind )
    {
    case Kind::Paused:        title = "Paused";          line1 = "ESC  continue";   line2 = "Q  back to the title screen"; break;
    case Kind::LevelComplete: title = "Level complete!"; line1 = buf;               line2 = "ENTER  next level"; break;
    case Kind::GameOver:      title = "Game over";       line1 = buf;               line2 = "ENTER  try again"; break;
    case Kind::Victory:       title = "You win!";        line1 = buf;               line2 = "ENTER  play again"; break;
    }

    const sf::Color gold( 255, 210, 80 );
    sf::Text t = makeText( loadFont( ctx, TitleFont ), title, 96, mKind == Kind::GameOver ? sf::Color( 255, 110, 90 ) : gold, 7.f );
    centerOrigin( t );
    t.setPosition( { size.x / 2.f, size.y * 0.38f } );
    target.draw( t );

    const auto& font = loadFont( ctx, TextFont );
    sf::Text l1 = makeText( font, line1, 56 );
    centerOrigin( l1 );
    l1.setPosition( { size.x / 2.f, size.y * 0.55f } );
    target.draw( l1 );

    sf::Text l2 = makeText( font, line2, 44 );
    centerOrigin( l2 );
    l2.setPosition( { size.x / 2.f, size.y * 0.65f } );
    target.draw( l2 );
}

} // namespace nomi
