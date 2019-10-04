#ifndef _SCENEVIEW_H_
#define _SCENEVIEW_H_

#include "dcscene_global.h"

#include "OsgQWidget/OsgQWidget.h"
#include "PickingHandler.h"
#include "StatisticsHandler.h"


#include <osgFX/Scribe>
#include <osgShadow/ShadowedScene>
#include <osg/AutoTransform>

class SceneModel;
class SelectionManager;

namespace DC
{
	class DCSCENE_EXPORT SceneView : public OsgQWidget
	{
		Q_OBJECT

	public:

		enum
		{
			PIVOT = 0,
			ZOOMIN,
			ZOOMOUT,
			DRAG
		};

		SceneView( QWidget * parent = NULL );
		virtual ~SceneView(void);

		void setSceneData(osg::Node *);
		void  setModel(SceneModel *);
		SceneModel* getModel();
		osgGA::TrackballManipulator *getTrackballManipulator(){return m_atm.get(); }

		double getLODFactor() const {return m_camera->getLODScale(); }
		void setLODFactor(double val);
		void setViewPoint(double distance,const osg::Vec3 & center, const osg::Matrixd& matrix);

		// stats
		void setStatsEnabled(bool val);

		void requestScreenShot();

		void setBackfaceEnabled(bool newbackface) {m_statesetManipulator->setBackfaceEnabled(newbackface); }
		bool getBackfaceEnabled() const {return m_statesetManipulator->getBackfaceEnabled(); }

		void setLightingEnabled(bool newlighting) { m_statesetManipulator->setLightingEnabled(newlighting); }
		bool getLightingEnabled() const {return m_statesetManipulator->getLightingEnabled(); }

		void setTextureEnabled(bool newtexture) {m_statesetManipulator->setTextureEnabled(newtexture); }
		bool getTextureEnabled() const {return m_statesetManipulator->getTextureEnabled(); }

		void setPolygonMode(osg::PolygonMode::Mode newpolygonmode) {m_statesetManipulator->setPolygonMode(newpolygonmode); }
		osg::PolygonMode::Mode getPolygonMode() const { return m_statesetManipulator->getPolygonMode(); }

		/** Get the master camera of the view. */
		osg::Camera* getCamera() { return m_camera.get(); }

		// accept picking geom or not
		void setEnabledSelection(bool val);
		void resetSelection();

		void resizeEvent(QResizeEvent * event);

		///获取背景色
		QColor getBgColor() const;
		osg::Vec4f getVec4BgColor() const;
		bool getGridEnabled() const {return m_showGrid; }

		public slots:

			void resetHome();
			void resetView(bool reset = true);
			void pickGeometry(osg::Drawable *d);

			void setFlatBackgroundColor(bool);
			void setGridEnabled(bool);

			///设置更新背景色
			void setBgColor(const QColor &color);
			void setShadowEnabled(bool val);
			void setCompassEnabled(bool val);
			bool isCompassEnabled() const;

			// pivot functions
			void recenterPivotPoint(double,double,double);
			void showPivot();
			void showZoomIn();
			void showZoomOut();
			void showDrag();

			void home();

			void setEnabledTrackbalHelper(bool);
			void setEnabledInverseMouseWheel(bool);

			bool highlight(osg::Node* node);

			bool centerOnNode(osg::Node* node);

signals:

			void newScreenshotAvailable(osg::Image *);

			void newAspectRatio(const QSize &);
			void picked(osg::Drawable *);

	protected:

	private:

		///配置场景环境
		void createSceneEnvironnement();
		void createPivotManipulator();

		// background creation
		void setGradientBgColor();
		osg::Projection*  createBackground();
		osg::MatrixTransform *makeGrid();
		osg::MatrixTransform *makeAxis();
		osg::Camera* createCompass();

		osg::Matrix matrixListtoSingle(const osg::MatrixList &tmplist);

		///节点管理模型
		SceneModel *m_model;

		///重写的trackball类，可以方便的反转鼠标滚轮 \n
		///setEnabledInverseMouseWheel
		osg::ref_ptr<PickingHandler>  m_atm;

		bool m_resetHome;
		osg::Matrixd m_matrix;
		osg::Vec3d m_center;
		double m_distance;

		///背景色

		/**背景渐变节点 */
		osg::ref_ptr<osg::Projection> m_gradientBackground;

		/**背景渐变颜色数组 */
		osg::Vec4Array *m_bgColors;

		/**背景色节点 */
		osg::ref_ptr<osg::ClearNode> m_backdrop;
		/**背景色 */
		osg::Vec4f m_clearColor;

		/**场景根节点 */
		osg::ref_ptr<osg::Group> m_scene;

		/**辅助网格节点 */
		osg::ref_ptr<osg::MatrixTransform> m_grid;

		/**辅助坐标轴节点 */
		osg::ref_ptr<osg::MatrixTransform> m_axis;

		/**是否显示辅助网格 */
		bool m_showGrid;

		/**辅助网格缩放因子 */
		float m_currGridScale;

		///model中根节点
		osg::ref_ptr<osgShadow::ShadowedScene> m_rootNodes;

		///指南针
		osg::ref_ptr<osg::Camera>      m_cameraCompass;

		// pivot
		osg::ref_ptr<osg::AutoTransform> m_rotatePivot;

		SelectionManager *m_selectionManager;
		osg::ref_ptr<StatisticsHandler> m_statsHandler;
	};
}



#endif // _SCENEVIEW_H_
