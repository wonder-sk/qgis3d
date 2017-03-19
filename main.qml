
import Qt3D.Core 2.0     // entity
import Qt3D.Render 2.0   // camera
import Qt3D.Input 2.0    // inputsettings
import Qt3D.Extras 2.0   // forwardrenderer

import qgis3d 1.0

Entity {
    id: sceneRoot

    Camera {
        id: mainCamera
        projectionType: CameraLens.PerspectiveProjection
        fieldOfView: 45
        aspectRatio: _window.width / _window.height
        nearPlane: 0.1
        farPlane: 1000.0
        position: Qt.vector3d(0.0, 10.0, 20.0)
        viewCenter: Qt.vector3d(0.0, 0.0, 0.0)  // the point at which camera is looking
        upVector: Qt.vector3d(0.0, 1.0, 0.0)  // orientation of camera - where is "up" direction... our plane has y=0, so up vector is with y>0
    }

    // Render from the mainCamera
    components: [
        RenderSettings {
            activeFrameGraph: ForwardRenderer {
                id: renderer
                camera: mainCamera
            }
        },
        // Event Source will be set by the Qt3DQuickWindow
        InputSettings { }
    ]

    FirstPersonCameraController { camera: mainCamera }


    // our custom mesh

    Entity {
        id: plane

        DiffuseMapMaterial {
            id: material
            ambient:  Qt.rgba(1,1,1,1)
            diffuse: "qrc:/ap.tif"
        }

        MyMesh {
            id: groundMesh
            width: 50
            height: width
            meshResolution: Qt.size(150, 150)
        }

        Transform {
            id: groundTransform
            translation: Qt.vector3d(0, -5, 0)
        }

        components: [
            groundMesh,
            groundTransform,
            material
        ]
    }

}
