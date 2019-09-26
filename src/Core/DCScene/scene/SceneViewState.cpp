#include "SceneViewState.h"

#include "SceneView.h"

SceneViewState::SceneViewState() :
    m_distance(0.0f),
    m_currRoot(NULL)
{}

SceneViewState::SceneViewState(DC::SceneView *view) :
    m_viewMatrix( view->getTrackballManipulator()->getMatrix() ),
    m_center( view->getTrackballManipulator()->getCenter() ),
    m_distance( view->getTrackballManipulator()->getDistance() ),
    m_currRoot(NULL)
{}

SceneViewState::SceneViewState(const SceneViewState& mystate) :
    m_viewMatrix( mystate.getViewMatrix() ),
    m_center( mystate.getCenter() ),
    m_distance( mystate.getDistance() ),
    m_currRoot( mystate.getRoot() )
{}

SceneViewState::~SceneViewState()
{}
