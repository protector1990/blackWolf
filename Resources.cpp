#include "Resources.hpp"
#include <iostream>

Resources::Resources():
    whitePrefix("Blue2"),
    blackPrefix("Blue"),
    boardSuffix("Black"),
    pathToPieces("Graphics/Pieces/")
{
    load();    
}

const sf::Texture &Resources::typeToTexture(Unit piece) const
{
    if (piece.color == Color::White){
        return *whiteRook;
    }else if (piece.color == Color::Black){
        return *blackRook;
    }



//    switch (piece) {
//    default: //appease compiler
//    case bw::Rook | bw::White:
//        return *whiteRook;
//    case bw::Bishop | bw::White:
//        return *whiteBishop;
//    case bw::Knight | bw::White:
//        return *whiteKnight;
//    case bw::Queen | bw::White:
//        return *whiteQueen;
//    case bw::Pawn | bw::White:
//        return *whitePawn;
//    case bw::King | bw::White:
//        return *whiteKing;
//    case bw::Rook | bw::Black:
//        return *blackRook;
//    case bw::Bishop | bw::Black:
//        return *blackBishop;
//    case bw::Knight | bw::Black:
//        return *blackKnight;
//    case bw::Queen | bw::Black:
//        return *blackQueen;
//    case bw::Pawn | bw::Black:
//        return *blackPawn;
//    case bw::King | bw::Black:
//        return *blackKing;
//    case bw::None: //let None denote the board for now
        return *board;

}

void Resources::load()
{
    thor::ResourceKey<sf::Texture> blackRookT = thor::Resources::fromFile<sf::Texture>(pathToPieces + blackPrefix + "R.png");
    thor::ResourceKey<sf::Texture> blackBishopT = thor::Resources::fromFile<sf::Texture>(pathToPieces + blackPrefix + "B.png");
    thor::ResourceKey<sf::Texture> blackKnightT = thor::Resources::fromFile<sf::Texture>(pathToPieces + blackPrefix + "N.png");
    thor::ResourceKey<sf::Texture> blackQueenT = thor::Resources::fromFile<sf::Texture>(pathToPieces + blackPrefix + "Q.png");
    thor::ResourceKey<sf::Texture> blackKingT = thor::Resources::fromFile<sf::Texture>(pathToPieces + blackPrefix + "K.png");
    thor::ResourceKey<sf::Texture> blackPawnT = thor::Resources::fromFile<sf::Texture>(pathToPieces + blackPrefix + "P.png");
    thor::ResourceKey<sf::Texture> whiteRookT = thor::Resources::fromFile<sf::Texture>(pathToPieces + whitePrefix + "R.png");
    thor::ResourceKey<sf::Texture> whiteBishopT = thor::Resources::fromFile<sf::Texture>(pathToPieces + whitePrefix + "B.png");
    thor::ResourceKey<sf::Texture> whiteKnightT = thor::Resources::fromFile<sf::Texture>(pathToPieces + whitePrefix + "N.png");
    thor::ResourceKey<sf::Texture> whiteQueenT = thor::Resources::fromFile<sf::Texture>(pathToPieces + whitePrefix + "Q.png");
    thor::ResourceKey<sf::Texture> whiteKingT = thor::Resources::fromFile<sf::Texture>(pathToPieces + whitePrefix + "K.png");
    thor::ResourceKey<sf::Texture> whitePawnT = thor::Resources::fromFile<sf::Texture>(pathToPieces + whitePrefix + "P.png");

    thor::ResourceKey<sf::Texture> boardTexture = thor::Resources::fromFile<sf::Texture>("Graphics/Board" + boardSuffix + ".jpg");

    try
    {

        board = cache.acquire(boardTexture);
    }
    catch (thor::ResourceLoadingException& e)
    {
        std::cerr << e.what() << std::endl;
        //throw - the gui must catch it
    }
}

void Resources::reload(const std::string &theWhitePrefix, const std::string &theBlackPrefix, const std::string &theBoardSuffix)
{
    whitePrefix = theWhitePrefix;
    blackPrefix = theBlackPrefix;
    boardSuffix = theBoardSuffix;

    load();
}

boost::property_tree::ptree Resources::getTree() const
{
    boost::property_tree::ptree pt;
    pt.put("whitePrefix",whitePrefix);
    pt.put("blackPrefix",blackPrefix);
    pt.put("boardSuffix",boardSuffix);
    return pt;
}
