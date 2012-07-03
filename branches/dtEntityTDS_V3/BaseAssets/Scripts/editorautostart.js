include_once("Scripts/osgveclib.js");
include_once("Scripts/stdlib.js");
include_once("Scripts/editormotionmodel.js");
include_once("Scripts/manipulators.js");


// create editor motion system when camera is created
function editorOnCameraAdded(name, params) {

  var camid = params.AboutEntity;
  var cam = cameraSystem.getComponent(camid);
  if(cam && cam.ContextId === 0) {
     if(!getEntitySystem("EditorMotion").hasComponent(camid)) {
        getEntitySystem("EditorMotion").createComponent(camid);
     }
  }
}
EntityManager.registerForMessages("CameraAddedMessage", editorOnCameraAdded);
