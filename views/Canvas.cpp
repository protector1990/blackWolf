#include "Canvas.hpp"
#include <boost/bimap/support/lambda.hpp>
#include <boost/assert.hpp>
#include <boost/cast.hpp>
#include "../controller/Controller.hpp"
//#include <boost/range/adaptor/filtered.hpp>

const sf::Vector2f Canvas::offToCenter(25.f,25.f);

Canvas::Canvas(sf::Window& theWindow, Resources& theResources):
    flipOffset(0),
    window(sfg::Canvas::Create()),
    bigWindow(theWindow),
    resources(theResources),
    currentPiece(pieces.end()),
    idCount(1)
{
    boardSprite_.setTexture(resources.typeToTexture({Color::None, Piece::None}));

    piecesTexture.loadFromFile("Graphics/Pieces/ArrayBlack.png");

    particle.loadFromFile("Graphics/particle.png");
    system.reset(new thor::ParticleSystem());
    system->setTexture(particle);
    system->addAffector(FireworkAffector());

    window->SetRequisition(sf::Vector2f( 440.f, 440.f ));
    window->GetSignal(sfg::Widget::OnMouseLeftPress).Connect(std::bind(&Canvas::slotLeftClick, this));
    window->GetSignal(sfg::Widget::OnMouseMove).Connect(std::bind(&Canvas::slotMouseMove, this));
    window->GetSignal(sfg::Widget::OnMouseLeftRelease).Connect(std::bind(&Canvas::slotMouseRelease, this));
    window->GetSignal(sfg::Widget::OnMouseEnter).Connect(std::bind(&Canvas::slotEnterCanvas, this));

    messages.connect("moveMade", [this](const Message& message){
        const MoveMessage* received = boost::polymorphic_downcast<const MoveMessage*>(&message);
        moveMake(received->move);
    });

    messages.connect("newGame", [this](const Message& message){
        const NewGameMessage* received = boost::polymorphic_downcast<const NewGameMessage*>(&message);
        resetFor(received->user);
        setPosition(Position());
    });

    messages.connect("endGame", [this](const Message& message){
        const EndGameMessage* received = boost::polymorphic_downcast<const EndGameMessage*>(&message);
        setResult(received->result);
    });
}

bool Canvas::flipped() const
{
    return (flipOffset!=0);
}

Square Canvas::toGridPos(const sf::Vector2f &position) const
{
    if (!flipped())
        return {static_cast<int>(std::ceil((position.y+7*flipOffset-370)/(2*flipOffset-50))),
                      static_cast<int>(std::floor((position.x-7*flipOffset-20)/(50-2*flipOffset)))};
    else
        return {static_cast<int>(std::floor((position.y+7*flipOffset-370)/(2*flipOffset-50))),
                      static_cast<int>(std::ceil((position.x-7*flipOffset-20)/(50-2*flipOffset)))};
}

bool Canvas::pieceHeld()
{
    return currentPiece != pieces.end();
}

sf::Vector2f Canvas::cellToPosition(const Square &square) const
{
    return sf::Vector2f(flipOffset * (7 - 2*square.col) + 20 + 50 * square.col,
                        -flipOffset * (7 - 2*square.row) + 420 - 50 * (square.row+1));
}

void Canvas::releasePiece()
{
    currentPiece = pieces.end();
}

Color Canvas::getColorOn(const Square &square) const
{
    return pieces.by<squareId>().find(square)->get<pieceId>().getColor();
}

void Canvas::destroy(const Square& square)
{
    if (pieces.by<squareId>().erase(square)) //if true then firework
        system->addEmitter(FireworkEmitter(cellToPosition(square) + offToCenter), sf::seconds(1.f));
}

void Canvas::display()
{
    system->update(frameClock.restart());

    window->Clear();

    window->Draw(boardSprite_);
    window->Draw(*system, sf::BlendAdd);

    vertexArray.clear();
    for (const auto& piece : pieces)
        piece.get<pieceId>().appendQuadTo(vertexArray);

    window->Draw(vertexArray.data(), vertexArray.size(), sf::Quads, &piecesTexture);



    for (auto& arrow : arrows)
        window->Draw(arrow, sf::BlendAdd);


}

void Canvas::moveMake(const CompleteMove& move)
{
    const Move& toMake(move.getMove());

    destroy(toMake.square_2);
    bimapMove(toMake);
    pieces.by<squareId>().find(toMake.square_2)->get<pieceId>().setPosition(cellToPosition(toMake.square_2));

    Position currentPosition = move.getNewBoard();
    if (currentPosition.wasCastle) handleCastle(toMake.square_2);
    if (currentPosition.wasEnPassant) handleEnPassant(toMake.square_2);
}

void Canvas::bimapMove(const Move &move)
{
    BOOST_VERIFY(pieces.by<squareId>().modify_key(pieces.by<squareId>().find(move.square_1), boost::bimaps::_key = move.square_2));
}

sfg::Widget::Ptr Canvas::getBoardWidget() const
{
    return window;
}

sf::Vector2f Canvas::getMousePosition() const
{
    return (static_cast<sf::Vector2f>(sf::Mouse::getPosition(bigWindow)) - window->GetAbsolutePosition());
}

void Canvas::sendBack()
{
    BOOST_ASSERT_MSG(pieceHeld(), "No current piece to send back");

    currentPiece->get<pieceId>().setPosition(cellToPosition(currentPiece->get<squareId>()));

    releasePiece();
}

void Canvas::flipBoard()
{
    if (flipped()) flipOffset = 0;
    else flipOffset = 50;

    for (const auto &piece : pieces)
        piece.get<pieceId>().setPosition(cellToPosition(piece.get<squareId>()));
}

void Canvas::reload(const Position &givenPosition)
{
    pieces.clear();
    setPosition(givenPosition);
    boardSprite_.setTexture(resources.typeToTexture({Color::None, Piece::None}));
}

void Canvas::setResult(Color result)
{
    for (const auto& piece : pieces){
        if (!(piece.get<pieceId>().getColor() & result)){
            system->addEmitter(FireworkEmitter(piece.get<pieceId>().getPosition() + offToCenter), sf::seconds(1.f));
            //pieces.erase(piece);
        }
    }
}

void Canvas::setArrow(const Move &move)
{
    sf::Vector2f point1 = cellToPosition(move.square_1) + offToCenter;
    sf::Vector2f point2 = cellToPosition(move.square_2) + offToCenter;

    arrows.emplace_back(point1,point2-point1,sf::Color(0,100,0,125), 5.f);
}

void Canvas::clearArrows()
{
    arrows.clear();
}

void Canvas::handleCastle(const Square &square)
{
    if (square.row==0){
        if (square.col==2){
            bimapMove({{0,0},{0,3}});
            pieces.by<squareId>().find(Square{0,3})->get<pieceId>().setPosition(cellToPosition({0,3}));
        }else{
            BOOST_ASSERT_MSG(square.col==6, "Invalid Castle");
            bimapMove({{0,7},{0,5}});
            pieces.by<squareId>().find(Square{0,5})->get<pieceId>().setPosition(cellToPosition({0,5}));
        }
    }else{
        BOOST_ASSERT_MSG(square.row==7, "Invalid Castle");
        if (square.col==2){
            bimapMove({{7,0},{7,3}});
            pieces.by<squareId>().find(Square{7,3})->get<pieceId>().setPosition(cellToPosition({7,3}));
        }else{
            BOOST_ASSERT_MSG(square.col==6, "Invalid Castle");
            bimapMove({{7,7},{7,5}});
            pieces.by<squareId>().find(Square{7,5})->get<pieceId>().setPosition(cellToPosition({7,5}));
        }
    }
}

void Canvas::handleEnPassant(const Square &square)
{
    if (square.row==5){
        destroy({4,square.col});
    }else{
        BOOST_ASSERT_MSG(square.row==2, "Invalid en passant");
        destroy({3,square.col});
    }
}

void Canvas::slotLeftClick()
{
    //if (gameEnded) return; - it should be disabled from outside
    //if (humanColor != getTurnColor()) return; - boardMaster will decide that

    sf::Vector2f clickedPoint = getMousePosition();

    //const int whoseTurn = getTurnColor();

    for (const auto &piece : pieces){
        //if (piece.getSide()!=whoseTurn) continue;
        if (piece.get<pieceId>().contains(clickedPoint)){
            currentPiece = pieces.project_up(pieces.by<pieceId>().find(piece.get<pieceId>()));
            pieceOffset = currentPiece->get<pieceId>().getPosition() - clickedPoint;
            break;
        }
    }
}

void Canvas::slotMouseMove()
{
    if (pieceHeld()){
        currentPiece->get<pieceId>().setPosition(getMousePosition() + pieceOffset);
    }
}

void Canvas::slotMouseRelease()
{
    if (pieceHeld()){
        sf::Vector2f centrePos = currentPiece->get<pieceId>().getPosition() + offToCenter;

        Square gridPos = toGridPos(centrePos);

        if (!(*requestMove({currentPiece->get<squareId>(), gridPos})))
            sendBack();
        else releasePiece();
    }
}

void Canvas::slotEnterCanvas()
{
    if (pieceHeld()){
        if (!sf::Mouse::isButtonPressed(sf::Mouse::Left)) sendBack();
    }
}

void Canvas::setPosition(const Position& givenPosition)
{
    for (int i=0; i<8; ++i){
        for (int j=0; j<8; ++j){
            const Unit pieceId = givenPosition({i, j});
            if ((pieceId.piece == Piece::None)||(pieceId.piece == Piece::Shadow)) continue;
            PieceSprite toAdd(cellToPosition({i,j}),pieceId, idCount++);
            pieces.insert(SquaresToPieces::value_type({i,j}, toAdd));
        }
    }

}

void Canvas::resetFor(Color whoFaceUp)
{
    if (flipped()){
        if (whoFaceUp & Color::White) flipBoard();
    }else{
        if (!(whoFaceUp & Color::White)) flipBoard();
    }    

    idCount = 1;
    pieces.clear();
}

boost::signals2::signal<bool (const Move&)>& Canvas::getSignal()
{
    return requestMove;
}

void Canvas::setPromotion(const Square &square, Piece piece)
{
    const Color whichSide = pieces.by<squareId>().find(square)->get<pieceId>().getColor();
    destroy(square);
    PieceSprite toAdd(cellToPosition(square),{whichSide, piece},idCount++);
    pieces.insert(SquaresToPieces::value_type(square, toAdd));
    //pieces[square].insert(toAdd);
}
