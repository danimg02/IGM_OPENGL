#include <iostream>
#include <vector>

#include <osg/Group>
#include <osg/Geode>
#include <osg/ShapeDrawable>
#include <osg/Geometry>
#include <osg/PositionAttitudeTransform>
#include <osgViewer/Viewer>
#include <osgGA/GUIEventHandler>
#include <osgGA/CameraManipulator>


class FixedCameraManipulator : public osgGA::CameraManipulator {
public:
    FixedCameraManipulator(const osg::Vec3& eye, const osg::Vec3& center, const osg::Vec3& up)
      : _eye(eye), _center(center), _up(up) {}

    virtual void setByMatrix(const osg::Matrixd&) override {}
    virtual void setByInverseMatrix(const osg::Matrixd&) override {}
    virtual osg::Matrixd getMatrix() const override {
        return osg::Matrixd::inverse(osg::Matrixd::lookAt(_eye, _center, _up));
    }
    virtual osg::Matrixd getInverseMatrix() const override {
        return osg::Matrixd::lookAt(_eye, _center, _up);
    }
    virtual bool handle(const osgGA::GUIEventAdapter&, osgGA::GUIActionAdapter&) override {
        return false;
    }

private:
    osg::Vec3 _eye, _center, _up;
};

class CameraSwitcher : public osgGA::GUIEventHandler {
public:
    CameraSwitcher(osgViewer::Viewer* viewer)
      : _viewer(viewer), _activeCamera(0)
    {
        _eyes.push_back(osg::Vec3(0.0f, -4.0f, 0.0f));
        _ups .push_back(osg::Vec3(0.0f,  0.0f, 1.0f));

        _eyes.push_back(osg::Vec3(0.0f, -0.01f, 5.0f));
        _ups .push_back(osg::Vec3(0.0f,  0.0f, 1.0f));

        _eyes.push_back(osg::Vec3(4.0f, -4.0f, 4.0f));
        _ups .push_back(osg::Vec3(0.0f,  0.0f, 1.0f));

        _center = osg::Vec3(0.0f, 0.0f, 0.0f);
        setManipulator(_activeCamera);
    }

    bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter&) override {
        if (ea.getEventType() == osgGA::GUIEventAdapter::KEYDOWN) {
            char k = ea.getKey();
            if (k >= '1' && k <= '3') {
                _activeCamera = k - '1';
                setManipulator(_activeCamera);
                return true;
            }
        }
        return false;
    }

private:
    void setManipulator(int idx) {
        osg::ref_ptr<FixedCameraManipulator> manip = new FixedCameraManipulator(_eyes[idx], _center, _ups[idx]);
        _viewer->setCameraManipulator(manip.get());
    }

    osgViewer::Viewer*      _viewer;
    std::vector<osg::Vec3>  _eyes;
    std::vector<osg::Vec3>  _ups;
    osg::Vec3               _center;
    int                     _activeCamera;
};

osg::ref_ptr<osg::Group> createScene() {
    osg::ref_ptr<osg::Group> root = new osg::Group();

    {
        osg::ref_ptr<osg::Box> box = new osg::Box(osg::Vec3(), 1.0f);
        osg::ref_ptr<osg::ShapeDrawable> sd = new osg::ShapeDrawable(box);
        sd->setColor(osg::Vec4(1,0,0,1));
        osg::ref_ptr<osg::Geode> geode = new osg::Geode();
        geode->addDrawable(sd.get());
        osg::ref_ptr<osg::PositionAttitudeTransform> pat = new osg::PositionAttitudeTransform();
        pat->setPosition(osg::Vec3(-1.5f, 0.0f, 0.0f));
        pat->addChild(geode.get());
        root->addChild(pat.get());
    }


    {
        osg::ref_ptr<osg::Vec3Array> verts = new osg::Vec3Array();
        verts->push_back(osg::Vec3(-0.5f,-0.5f,-0.5f));
        verts->push_back(osg::Vec3( 0.5f,-0.5f,-0.5f));
        verts->push_back(osg::Vec3( 0.5f, 0.5f,-0.5f));
        verts->push_back(osg::Vec3(-0.5f, 0.5f,-0.5f));
        verts->push_back(osg::Vec3( 0.0f, 0.0f, 0.5f));

        osg::ref_ptr<osg::DrawElementsUInt> idx = new osg::DrawElementsUInt(GL_TRIANGLES);
        // base
        idx->push_back(0); idx->push_back(1); idx->push_back(2);
        idx->push_back(0); idx->push_back(2); idx->push_back(3);
        // lados
        idx->push_back(0); idx->push_back(1); idx->push_back(4);
        idx->push_back(1); idx->push_back(2); idx->push_back(4);
        idx->push_back(2); idx->push_back(3); idx->push_back(4);
        idx->push_back(3); idx->push_back(0); idx->push_back(4);

        osg::ref_ptr<osg::Geometry> geom = new osg::Geometry();
        geom->setVertexArray(verts.get());
        geom->addPrimitiveSet(idx.get());
        osg::ref_ptr<osg::Vec4Array> cols = new osg::Vec4Array();
        cols->push_back(osg::Vec4(0,1,0,1));
        geom->setColorArray(cols.get(), osg::Array::BIND_OVERALL);

        osg::ref_ptr<osg::Geode> geode = new osg::Geode();
        geode->addDrawable(geom.get());
        root->addChild(geode.get());
    }

    {
        osg::ref_ptr<osg::Sphere> sphere = new osg::Sphere(osg::Vec3(), 0.5f);
        osg::ref_ptr<osg::ShapeDrawable> sd = new osg::ShapeDrawable(sphere);
        sd->setColor(osg::Vec4(0,0,1,1));
        osg::ref_ptr<osg::Geode> geode = new osg::Geode();
        geode->addDrawable(sd.get());
        osg::ref_ptr<osg::PositionAttitudeTransform> pat = new osg::PositionAttitudeTransform();
        pat->setPosition(osg::Vec3(1.5f, 0.0f, 0.0f));
        pat->addChild(geode.get());
        root->addChild(pat.get());
    }

    return root;
}

int main() {
    osgViewer::Viewer viewer;
    viewer.setSceneData(createScene().get());
    viewer.getCamera()->setProjectionMatrixAsPerspective(45.0, 800.0/600.0, 0.1, 100.0);
    viewer.setCameraManipulator(nullptr);
    viewer.addEventHandler(new CameraSwitcher(&viewer));
    return viewer.run();
}
