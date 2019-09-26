#include "PrintPosterHandler.h"

PrintPosterHandler::PrintPosterHandler()
    :   _isRunning(false), _isFinished(false),
    _outputTiles(false), _outputTileExt("bmp"),
    _currentRow(0), _currentColumn(0),
    _cameraIndex(0), _cameraRoot(0), _finalPoster(0), start_(false)
{}

bool PrintPosterHandler::handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa )
{
    osgViewer::View* view = dynamic_cast<osgViewer::View*>( &aa );
    if ( !view )
        return false;

    switch( ea.getEventType() )
    {
        case osgGA::GUIEventAdapter::FRAME:
        {
            if (start_)
            {
                _tileRows = (int)( _posterSize.y() / _tileSize.y() );
                _tileColumns = (int)( _posterSize.x() / _tileSize.x() );
                _currentRow = 0;
                _currentColumn = 0;
                _cameraIndex = _cameraRoot->getNumChildren();
                _currentViewMatrix = view->getCamera()->getViewMatrix();
                _currentProjectionMatrix = view->getCamera()->getProjectionMatrix();
                _images.clear();

                _isRunning = true;
                _isFinished = false;
                start_ = false;
            }

            if ( view->getDatabasePager() )
            {
                // Wait until all paged LOD are processed
                if ( view->getDatabasePager()->getRequestsInProgress() )
                    break;
            }

            if ( _isFinished )
            {
                const osg::FrameStamp* fs = view->getFrameStamp();
                if ( (fs->getFrameNumber() % 2)==0 )
                {
                    // Record images and unref them to free memory
                    recordImages();
                }
            }

            if ( _isRunning )
            {
                // Every "copy-to-image" process seems to be finished in 2 frames.
                // So record them and dispatch cameras to next tiles.
                const osg::FrameStamp* fs = view->getFrameStamp();
                if ( (fs->getFrameNumber() % 2)==0 )
                {
                    // Record images and unref them to free memory
                    recordImages();

                    osg::Camera* camera = 0;
                    while ( ( camera = getAvailableCamera() )!=NULL )
                    {
                        std::cout << "Binding sub-camera " << _currentRow << "_" << _currentColumn
                                  << " to image..." << std::endl;
                        bindCameraToImage( camera, _currentRow, _currentColumn );
                        if ( _currentColumn<_tileColumns - 1 )
                            _currentColumn++;
                        else
                        {
                            if ( _currentRow<_tileRows - 1 )
                            {
                                _currentRow++;
                                _currentColumn = 0;
                            }
                            else
                            {
                                _isRunning = false;
                                _isFinished = true;
                                std::cout << "Sub-cameras dispatching finished." << std::endl;
                                break;
                            }
                        }
                    }
                    _cameraIndex = _cameraRoot->getNumChildren();
                }
            }
            break;
        }
        default:
            break;
    }

    return false;
}

osg::Camera* PrintPosterHandler::getAvailableCamera()
{
    // Find an available camera for rendering current tile image.
    if ( !_cameraIndex || !_cameraRoot.valid() )
        return NULL;

    return dynamic_cast<osg::Camera*>( _cameraRoot->getChild(--_cameraIndex) );
}

void PrintPosterHandler::bindCameraToImage( osg::Camera* camera, int row, int col )
{
    std::stringstream stream;
    stream << "image_" << row << "_" << col;

    osg::ref_ptr<osg::Image> image = new osg::Image;
    image->setName( stream.str() );
    image->allocateImage( (int)_tileSize.x(), (int)_tileSize.y(), 1, GL_RGBA, GL_UNSIGNED_BYTE );
    _images[TilePosition(row,col)] = image.get();

    // Calculate projection matrix offset of each tile
    osg::Matrix offsetMatrix =
        osg::Matrix::scale(_tileColumns, _tileRows, 1.0) *
        osg::Matrix::translate(_tileColumns - 1 - 2 * col, _tileRows - 1 - 2 * row, 0.0);
    camera->setViewMatrix( _currentViewMatrix );
    camera->setProjectionMatrix( _currentProjectionMatrix * offsetMatrix );

    // Reattach cameras and new allocated images
    camera->setRenderingCache( NULL );      // FIXME: Uses for reattaching camera with image, maybe inefficient?
    camera->detach( osg::Camera::COLOR_BUFFER );
    camera->attach( osg::Camera::COLOR_BUFFER, image.get(), 0, 0 );
}

void PrintPosterHandler::recordImages()
{
    for ( TileImages::iterator itr = _images.begin(); itr!=_images.end(); ++itr )
    {
        osg::Image* image = (itr->second).get();
        if ( _finalPoster.valid() )
        {
            // FIXME: A stupid way to combine tile images to final result. Any better ideas?
            unsigned int row = itr->first.first, col = itr->first.second;
            for ( int s = 0; s<image->s(); ++s )
                for ( int t = 0; t<image->t(); ++t )
                {
                    unsigned char* src = image->data(s, t);
                    unsigned char* target = _finalPoster->data( s + col * (int)_tileSize.x(), t + row * (int)_tileSize.y() );
                    for ( int u = 0; u<4; ++u )
                        *(target + u) = *(src++);
                }
        }

        if ( _outputTiles )
            osgDB::writeImageFile( *image, image->getName() + "." + _outputTileExt );
    }
    _images.clear();
}
