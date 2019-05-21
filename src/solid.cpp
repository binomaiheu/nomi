#include <iostream>

#include "solid.hpp"

namespace nomi {


Solid::Solid( const tmx::Object &obj )
    : SceneNode( SceneNode::Type::Solid )
{

    // creating 
    std::cout << "Creating object name= [" << obj.getName() 
              << "], type = " << obj.getType()
              << ", x= " << obj.getPosition().x 
              << ", y= " << obj.getPosition().y << "\n";
                    std::cout << "            aabb : " << obj.getAABB().left << ", " << obj.getAABB().top << ", " << obj.getAABB().width << ", " << obj.getAABB().height << "\n";
                    std::cout << "            points : ( ";
                    for ( auto& p : obj.getPoints() ) std::cout << " ( " << p.x << ", " << p.y << " )";
                    std::cout << ")\n";



    // set the shape
    if ( ( obj.getAABB().width > 0 ) && ( obj.getAABB().height > 0 ) ) {
        // rectangle shape
        std::cout << "Adding rectangle shape...\n";
        if ( ( obj.getAABB().left != obj.getPosition().x ) || ( obj.getAABB().top != obj.getPosition().y ) )
            throw std::runtime_error( "Coordinates do not match in tmx rectangle shape..." );

        mShape = std::move( std::make_unique<sf::RectangleShape>( sf::Vector2f( obj.getAABB().width, obj.getAABB().height ) ) );
    } else if ( ( obj.getAABB().width == 0 ) && ( obj.getAABB().height == 0 ) && 
                ( obj.getPoints().size() > 2 ) ) {
        // polygon shape
        std::cout << "Adding polygon shape...\n";
        sf::ConvexShape p;
        p.setPointCount( obj.getPoints().size() );
        for ( unsigned int i=0; i<obj.getPoints().size(); i++ ) p.setPoint( i, { obj.getPoints().at(i).x, obj.getPoints().at(i).y } );

        mShape = std::move( std::make_unique<sf::ConvexShape>( p ) );

    } else if ( ( obj.getAABB().width == 0 ) && ( obj.getAABB().height == 0 ) && 
                ( obj.getPoints().size() > 2 ) ) {
        // point
        throw std::runtime_error( "tmx point object geometry not implemented in solid class" );
    } else {
        throw std::runtime_error( "Error in tmx object geometry for object " + obj.getName() );
    }
    

    mShape->setOutlineColor(sf::Color::Red);
    mShape->setFillColor( sf::Color::Transparent );
    mShape->setOutlineThickness(1);


    // set the origin to the shape center
    sf::FloatRect bounds{mShape->getLocalBounds()};
    mShape->setOrigin(bounds.width /2.f, bounds.height /2.f);
    // adjust position accordingly    
    setPosition( { obj.getPosition().x+bounds.width /2.f, obj.getPosition().y+bounds.height /2.f } );
}

Solid::~Solid()
{}

void Solid::drawCurrent( sf::RenderTarget& target, sf::RenderStates states ) const
{

#define DEBUG_SOLID
#ifdef DEBUG_SOLID
    target.draw( *mShape, states );
#endif
}

void Solid::updateCurrent( sf::Time dt ) 
{
    // do nothing..
}

sf::FloatRect Solid::getBoundingRect() const
{
    return getWorldTransform().transformRect(mShape->getGlobalBounds());
}


} // namespace

