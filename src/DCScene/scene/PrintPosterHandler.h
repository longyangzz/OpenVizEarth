#ifndef _PRINTPOSTERHANDLER_H_
#define _PRINTPOSTERHANDLER_H_

#include <osg/Camera>
#include <osg/Texture2D>
#include <osgDB/WriteFile>
#include <osgViewer/Viewer>
#include <iostream>
#include <sstream>

class PrintPosterHandler :
    public osgGA::GUIEventHandler
{
public:

    typedef std::pair<unsigned int, unsigned int> TilePosition;
    typedef std::map< TilePosition, osg::ref_ptr<osg::Image> > TileImages;

    PrintPosterHandler();

    inline void setOutputTiles( bool b ) { _outputTiles = b; }
    inline bool getOutputTiles() const { return _outputTiles; }

    inline void setOutputTileExtension( const std::string& ext ) { _outputTileExt = ext; }
    inline const std::string& getOutputTileExtension() const { return _outputTileExt; }

    inline void setTileSize( int w, int h ) { _tileSize.set(w, h); }
    inline const osg::Vec2& getTileSize() const { return _tileSize; }

    inline void setPosterSize( int w, int h ) { _posterSize.set(w, h); }
    inline const osg::Vec2& getPosterSize() const { return _posterSize; }

    inline void setCameraRoot( osg::Group* root ) { _cameraRoot = root; }
    inline const osg::Group* getCameraRoot() const { return _cameraRoot.get(); }

    inline void setFinalPoster( osg::Image* image ) { _finalPoster = image; }
    inline const osg::Image* getFinalPoster() const { return _finalPoster.get(); }

    bool handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa );

    void start() {start_ = true; }

protected:
    osg::Camera* getAvailableCamera();

    void bindCameraToImage( osg::Camera* camera, int row, int col );

    void recordImages();

    bool _isRunning;
    bool _isFinished;

    bool _outputTiles;
    std::string _outputTileExt;

    osg::Vec2 _tileSize;
    osg::Vec2 _posterSize;

    int _tileRows;
    int _tileColumns;

    int _currentRow;
    int _currentColumn;
    unsigned int _cameraIndex;

    osg::Matrixd _currentViewMatrix;
    osg::Matrixd _currentProjectionMatrix;

    osg::ref_ptr<osg::Group> _cameraRoot;
    osg::ref_ptr<osg::Image> _finalPoster;
    TileImages _images;
    bool start_;
};

#endif // _PRINTPOSTERHANDLER_H_
