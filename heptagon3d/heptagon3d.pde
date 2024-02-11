import nervoussystem.obj.*;
import processing.dxf.*;
import peasy.*;
import queasycam.*;
import controlP5.*;
//import processing.video.*;
import toxi.geom.Vec3D;
import org.apache.commons.lang3.*;

final float PHI = .6180339887;

float diam = 1000;
float diamMM = 850;
int step = 39;
int holderSize = 5;
boolean recordObj = false;
boolean recordDxf = false;
int selected = -1;
boolean drawHalfes = false;
boolean drawEdges = false;
boolean drawMargins = false;
int config = 0;
int legLen = -318;
boolean orientWallToLeg = false;
float planeSize = diam / diamMM * 2500.f;
float edgesWidth = 7;
boolean showGui = false;
boolean showWall = false;
boolean showGlass = false;
boolean showFloor = false;
boolean showVideo = false;
boolean showInfo = true;
boolean showFrame = true;
boolean fillFrame = false;
boolean showHeptagon = true;
boolean showRoom = false;
float rotation = -2*PI/4.f + 2*PI/2.f;
float rotationTarget = rotation;
boolean regularLeg;
PVector regularLegDir = new PVector(0.0, 0.0, 1.0);
boolean extendLegToGlass = true;
boolean openTop = false;
boolean openBottom = true;
float distanceToFloor = diam / diamMM * 1275.f; //1500.f;
//String movieFile = "video.mov";
String movieFile = "ScreenFlow.mp4";
//float movieAspectRatio = 16.f / 9.f;
float movieAspectRatio = 1;
float movieScale = 1;
CodedKeys codedKeys = new CodedKeys();
char editDimension = 'z';
float eyesD = 1.5 * step;
float eyesR = 10;
float faceZ = -step;
int frameDepth = 200;

Heptagon heptagon;
Tunnel leg;
//Movie movie;
PeasyCam pCam;
//QueasyCam qCam;
ControlP5 cp5;

void setup() {
  //size(960, 540, P3D);
  fullScreen(P3D); 
  
  pCam = new PeasyCam(this, 500);
  pCam.setMinimumDistance(50);
  pCam.setMaximumDistance(10000);
  //qCam = new QueasyCam(this, 50.f, 10000.f);
  //qCam.controllable = false;
  
  heptagon = new Heptagon(diam);
  heptagon.insetWOZ = !regularLeg;
  leg = new Tunnel(diam / 5.f, legLen, 7);
  updateFromConfig();
  
  cp5 = new ControlP5(this);
  cp5.setAutoDraw(false);
  for (int i=0; i<12; i++) {
    cp5.addSlider("vert"+i+".x")
      .setValue(0)
      .setPosition(10, 10+i*20)
      .setRange(-20, 20)
      .setNumberOfTickMarks(41)
      .setVisible(false)
      .setId(100+i);
    cp5.addSlider("vert"+i+".y")
      .setValue(0)
      .setPosition(10, 10+i*20)
      .setRange(-20, 20)
      .setNumberOfTickMarks(41)
      .setVisible(false)
      .setId(200+i);
    cp5.addSlider("vert"+i+".z")
      .setValue(heptagon.hepts[0].verts[i].z / step)
      .setPosition(10, 10+i*20)
      .setRange(-4, 0)
      .setNumberOfTickMarks(5)
      .setId(300+i);
  }
  cp5.addSlider("edgesWidth")
    .setPosition(10, 10+12*20)
    .setRange(0, 30);
  cp5.addSlider("legLen")
    .setPosition(10, 10+13*20)
    .setRange(0, -500);
  cp5.addSlider("planeSize")
    .setPosition(10, 10+14*20)
    .setRange(0, 3000);
  cp5.addSlider("step")
    .setPosition(10, 10+15*20)
    .setRange(0, 100);
  cp5.addSlider("eyesD")
    .setPosition(10, 10+16*20)
    .setRange(0, 100);
  cp5.addSlider("faceZ")
    .setPosition(10, 10+17*20)
    .setRange(0, -4 * step);
  cp5.addSlider("frameDepth")
    .setPosition(10, 10+18*20)
    .setRange(30, 600);
    
  float x = width - 70;
  int i = 0;
    
  cp5.addToggle("regularLeg")
    .setPosition(x, 10+i*20);
  cp5.addToggle("openTop")
    .setPosition(x, 10+i*20+40);
  cp5.addToggle("openBottom")
    .setPosition(x, 10+i*20+2*40);  
  cp5.addToggle("extendLegToGlass")
    .setPosition(x, 10+i*20+3*40);
  cp5.addToggle("insetWOZ")
    .setPosition(x, 10+i*20+4*40);
  cp5.addToggle("showHeptagon")
    .setPosition(x, 10+i*20+5*40);
  cp5.addToggle("showFrame")
    .setPosition(x, 10+i*20+6*40);
  cp5.addToggle("showGlass")
    .setPosition(x, 10+i*20+7*40);
  cp5.addToggle("showWall")
    .setPosition(x, 10+i*20+8*40);
  cp5.addToggle("showFloor")
    .setPosition(x, 10+i*20+9*40);
  cp5.addToggle("showVideo")
    .setPosition(x, 10+i*20+10*40);
  cp5.addToggle("drawMargins")
    .setPosition(x, 10+i*20+11*40);
  cp5.addToggle("fillFrame")
    .setPosition(x, 10+i*20+12*40);
    
  //movie = new Movie(this, movieFile);
  //movie.play();
  //movie.loop();
  //movie.volume(0);
}

void updateFromConfig() {
  heptagon.updateSize();
  switch (config) {
    case 0: {
      regularLegDir = new PVector(0.4729239, 2.73638E-7, 0.8811033);
      float[][] config = {
        {0, 0, 0, -1, -2, -1, -4, -3, -2, -4, 0, -3},
        {0, 0, 0, -2, -2, -2, -3, -2, -1,  -3, 0, -2},
        {0, 0, 0, -3, -3, -3, -2, -1, -3, -2, 0, -1},
        {0, 0, 0, -4, -4, -4, -1, -1, -4, -1, 0, -1},
        {0, 0, 0, -3, -3, -3, -1, -2, -3, -1, 0, -2},
        {0, 0, 0, -2, -2, -2, -2, -3, -1, -2, 0, -3},
        {0, 0, 0, -1, -1, -2, -3, -4, -2, -3, 0, -4},
      };
      applyConfig(config);
      break;
    }
    case 1: {
      regularLegDir = new PVector(0.4729239, 2.73638E-7, 0.8811033);
      float[][] config = {
        {0, 0, 0, -2, -2, -2, -3, -2, -2, -3, 0, -2},
        {0, 0, 0, -3, -3, -3, -2, -1, -3,  -2, 0, -1},
        {0, 0, 0, -3, -3, -3, -1, 0, -3, -1, 0, 0},
        {0, 0, 0, -3, -3, -3, 0, 0, -3, 0, 0, 0},
        {0, 0, 0, -3, -3, -3, 0, -1, -3, 0, 0, -1},
        {0, 0, 0, -3, -3, -3, -1, -2, -3, -1, 0, -2},
        {0, 0, 0, -2, -2, -2, -2, -3, -2, -2, 0, -3},
      };
      applyConfig(config);
      break;
    }
    case 2: {
      regularLegDir = new PVector(0.4729239, 2.73638E-7, 0.8811033);
      float[][] config = {
        {0, 0, 0, -3, -3, -3, 0, -1, -3, 0, 0, -1},
        {0, 0, 0, -3, -3, -3, -1, -2, -3, -1, 0, -2},
        {0, 0, 0, -3, -3, -3, -2, -3, -3, -2, 0, -3},
        {0, 0, 0, -4, -4, -4, -3, -3, -4, -3, 0, -3},
        {0, 0, 0, -3, -3, -3, -3, -2, -3,  -3, 0, -2},
        {0, 0, 0, -3, -3, -3, -2, -1, -3, -2, 0, -1},
        {0, 0, 0, -3, -3, -3, -1, 0, -3, -1, 0, 0},
      };
      applyConfig(config);
      break;
    }
    case 3: {
      regularLegDir = new PVector(0.5819748, 3.788278E-7, 0.8132068);
      float[][] config = {
        {0, 0, 0, -2, -2, -2, -3, -2, -2, -4, 0, -2},
        {0, 0, 0, -3, -3, -3, -2, -1, -3,  -2, 0, -1},
        {0, 0, 0, -3, -3, -3, -1, 0, -3, -1, 0, 0},
        {0, 0, 0, -3, -3, -3, 0, 0, -3, 0, 0, 0},
        {0, 0, 0, -3, -3, -3, 0, -1, -3, 0, 0, -1},
        {0, 0, 0, -3, -3, -3, -1, -2, -3, -1, 0, -2},
        {0, 0, 0, -2, -2, -2, -2, -3, -2, -2, 0, -4},
      };
      applyConfig(config);
      break;
    }
    case 4: {
      regularLegDir = new PVector(0.0, 0.0, 1.0);
      for (int i=0; i<12; i++) {
        int z = (i==3 || i==4 || i==5 || i==8) ? -3 * step : 0;
        heptagon.setHeptsZ(i, z);
      }
      break;
    }
    default:
      float config[][] = {};
      applyConfig(config);
      break;
  }
  updateLeg();
}

void applyConfig(float[][] config) {
  for (int i=0; i<7; i++) {
    heptagon.hepts[i].reset();
    if (config.length > i) {
      for (int j=0; j<12; j++) {
        if (!regularLeg) {
          heptagon.hepts[i].verts[j].z = config[i][j] * step;
        }
        else {
          heptagon.insetWOZ = false;
          heptagon.hepts[i].verts[j].add(PVector.mult(regularLegDir, config[i][j] * step));
        }
      }
    }
  }
}

void updateLeg() {
  leg.length = legLen;
  if (regularLeg) {
    leg.reset();
    leg.rotation = regularLegDir;
    leg.update();
    if (extendLegToGlass) {
      for (int i=0; i<7; i++) {
        PVector v1 = leg.verts[i][0];
        PVector v2 = leg.verts[i][1];
        PVector v1Rot = new PVector(v1.x, 0).rotate(heading3D(leg.rotation));
        PVector v2Rot = new PVector(v2.x, 0).rotate(heading3D(leg.rotation));
        float z1 = sqrt(pow(v1Rot.y, 2) + pow(v1.x - v1Rot.x, 2)) * signum(v1.x);
        float z2 = sqrt(pow(v2Rot.y, 2) + pow(v2.x - v2Rot.x, 2)) * signum(v2.x);
        v1.add(new PVector(0, 0, z1));
        v2.add(new PVector(0, 0, z2));
      }
    }
  }
  else {
    leg.rotation = surfaceNormal(heptagon.hepts[0].verts[9], heptagon.hepts[3].verts[9], heptagon.hepts[3].verts[11]);
  }
}

void updateGui() {
  char dims[] = {'x', 'y', 'z'};
  for (char d : dims) {
    for (int i=0; i<12; i++) {
      Controller control = cp5.getController("vert"+i+"."+d);
      PVector vert = heptagon.hepts[selected].verts[i];
      PVector origVert = heptagon.hepts[selected].origVerts[i];
      switch (d) {
        case 'x':
          control.setValue(vert.x - origVert.x);
          break;
        case 'y':
          control.setValue(vert.y - origVert.y);
          break;
        case 'z':
          control.setValue(vert.z / step);
          break;
      }
    }
  }
}

void update() {
  heptagon.inset = edgesWidth;
  heptagon.halfes = drawHalfes;
  heptagon.margins = drawMargins;
  heptagon.face.z = faceZ;
  heptagon.frame.depth = frameDepth;
  heptagon.update();
  leg.update();
  if (!regularLeg && leg.rotation != null) {
    leg.dir = leg.rotation.copy();
    leg.rotation = null;
    for (int i=0; i<7; i++) {
      PVector p1 = heptagon.hepts[i].verts[9].copy();
      PVector p2 = heptagon.hepts[i].verts[11].copy();
      if (openBottom) {
        if (i == 0) {
          //p1.add(PVector.sub(p2, p1).div(2.f));
          p1.set(p2);
        }
        else if (i == 6) {
          //p2.add(PVector.sub(p1, p2).div(2.f));
          p2.set(p1);
        }
      }
      if (extendLegToGlass) {
        p1.add(leg.dir.copy().mult(-p1.z / leg.dir.z));
        p2.add(leg.dir.copy().mult(-p2.z / leg.dir.z));
      }
      leg.updateVerts(i, p1, p2);
    }
  }
  rotation += (rotationTarget - rotation)*1/12.f;
}

void draw() {
  update();
  background(0);
  
  if (recordObj) {
    beginRecord("nervoussystem.obj.OBJExport", "data/heptagon.obj");
  }
  else if (recordDxf) {
    beginRaw(DXF, "data/heptagon.dxf");
  }
  else {
    lights();
  }
  
  pushMatrix();
  rotate(rotation);
  drawHeptagon();
  if (showGlass) {
    drawGlass();
  }
  if (heptagon.face.type == Face.TYPE_HOLLOW) {
    drawLeg();
  }
  drawPlanes(); // wall and floor
  if (showRoom) {
    drawRoom();
  }
  popMatrix();
  
  if (recordObj) {
    endRecord();
    recordObj = false;
    println("obj recording finished");
  }
  else if (recordDxf) {
    endRaw();
    recordDxf = false;
    println("dxf recording finished");
  }
  if (showInfo) {
    drawInfo();
  }
  if (showGui) {
    gui();
  }
}

void drawHeptagon() {
  if (showHeptagon) {
    for (int i=0; i<7; i++) {
      pushStyle();
      if (i == selected) {
        fill(255, 0, 0);
      }
      else {
        fill(255);
      }
      heptagon.hepts[i].draw();
      popStyle();
    }
    heptagon.face.draw();
  }
  if (fillFrame) {
    drawFilledFrame();
  }
  else if (showFrame) {
    heptagon.frame.draw();
  }
}

void drawFilledFrame() {
  pushStyle();
  fill(255, 255);
  pushMatrix();
  translate(0, 0, -frameDepth);
  beginShape(TRIANGLE_FAN);
  vertex(0, 0, 0);
  PVector p, p1;
  for (int i=0; i<7; i++) {
    p = pointOnEllipse(TWO_PI/7.f*i, diam, diam);
    vertex(p.x, p.y, 0);
  }
  p = pointOnEllipse(0, diam, diam);
  vertex(p.x, p.y, 0);
  endShape();
  popMatrix();
  beginShape(QUADS);
  for (int i=0; i<7; i++) {
    p = pointOnEllipse(TWO_PI/7.f*i, diam, diam);
    p1 = pointOnEllipse(TWO_PI/7.f*(i+1), diam, diam);
    vertex(p.x, p.y, 0);
    vertex(p1.x, p1.y, 0);
    vertex(p1.x, p1.y, -frameDepth);
    vertex(p.x, p.y, -frameDepth);
  }
  endShape();
  popStyle();
}

void drawLeg() {
  pushMatrix();
  pushStyle();
  fill(255, 255/2.f);
  leg.draw();
  popStyle();
  popMatrix();
}

void drawPlanes() {
  if (showWall) {
    pushMatrix();
    if (orientWallToLeg) {
      if (!regularLeg) {
        rotateY(heading3D(leg.dir));
      }
      else {
        rotateY(heading3D(leg.rotation));
      }
    }
    translate(0, 0, legLen);
    if (showVideo) {
      rotateZ(-2*PI/4.f);
      //image(movie, -planeSize/2.0 * movieAspectRatio * movieScale, -planeSize/2.0 * movieScale, planeSize * movieAspectRatio * movieScale, planeSize * movieScale);
    }
    else {
      rect(-planeSize/2.0, -planeSize/2.0, planeSize, planeSize);
    }
    popMatrix();
  }
  if (showFloor) {
    pushMatrix();
    if (orientWallToLeg) {
      if (!regularLeg) {
        rotateY(heading3D(leg.dir) + 2*PI/4.f);
      }
      else {
        rotateY(heading3D(leg.rotation) + 2*PI/4.f);
      }
    }
    else {
      rotateY(2*PI/4.f);
    }
    translate(-planeSize/2.f, 0, distanceToFloor);
    rect(-planeSize/2.0 - legLen, -planeSize/2.0, planeSize, planeSize);
    popMatrix();
  }
}

void drawRoom() {
  pushMatrix();
  translate(0, 0, planeSize/2);
  pushStyle();
  noFill();
  stroke(255);
  strokeWeight(5);
  box(planeSize, planeSize * PHI, planeSize);
  popStyle();
  popMatrix();
}

void drawGlass() {
  pushStyle();
  fill(255, 0, 0, 255/4);
  noStroke();
  if (!regularLeg) {
    if (heptagon.face.type == Face.TYPE_HOLLOW || openTop || openBottom) {
      PShape innerShape;
      if (heptagon.face.type == Face.TYPE_HOLLOW) {
        innerShape = leg.getGlassCutOut(openTop, openBottom);
      }
      else {
        innerShape = heptagon.face.getGlassCutOut(leg.verts, openTop, openBottom);
      }
      shape(subtract(heptagon(diam, true), innerShape, false));
    }
    else {
      shape(heptagon(diam, true));
    }
  }
  else {
    shape(subtract(heptagon(diam), heptagon(leg.diam), false));
  }
  popStyle();
}

void drawInfo() {
  float mmRatio = edgesWidth / 8;
  float side = (diam / 2.f * 0.86777);
  hint(DISABLE_DEPTH_TEST);
  pCam.beginHUD();
  textSize(12);
  text("diameter: " + diam + " / " + nf(mmRatio * diam / 10, 0, 2) + " cm", width - 250, 20);
  text("side: " + side + " / " + nf(mmRatio * side / 10, 0, 2) + " cm", width - 250, 40);
  text("edges: " + edgesWidth + " / 8 mm (x2)", width - 250, 60);
  text("length1: " + heptagon.getLength1() + " / " + nf(mmRatio * heptagon.getLength1() / 10, 0, 2) + " cm", width - 250, 80);
  text("length2: " + heptagon.getLength2() + " / " + nf(mmRatio * heptagon.getLength2() / 10, 0, 2) + " cm", width - 250, 100);
  if (showFrame) {
    if (heptagon.frame.rotation != 0) {
      text("frame lengths: " + StringUtils.join(ArrayUtils.toObject(heptagon.frame.lengthsMM), ", "), width - 500, 120);
    }
    else {
      text("frame length: " + heptagon.frame.lengthsMM[0], width - 250, 120);
    }
  }
  text("i - toggle info", width-250, 140);
  text("space - toggle gui", width-250, 160);
  text("s - save obj", width-250, 180);
  text("s - save dxf", width-250, 200);
  text("h - drawHalfes", width-250, 220);
  text("m - drawMargins", width-250, 240);
  text("u - regularLeg", width-250, 260);
  text("p - showWall", width-250, 280);
  text("d - showFloor", width-250, 300);
  text("g - showGlass", width-250, 320);
  text("f - showFrame", width-250, 340);
  text("e - toggleEdges", width-250, 360);
  text("c - switch cam", width-250, 380);
  text("q - extendLegToGlass", width-250, 400);
  text("v - showVideo", width-250, 420);
  text("n - insetWOZ", width-250, 440);
  text("o - next face", width-250, 460);
  pCam.endHUD();
  hint(ENABLE_DEPTH_TEST);
}

void gui() {
  hint(DISABLE_DEPTH_TEST);
  pCam.beginHUD();
  cp5.draw();
  pCam.endHUD();
  hint(ENABLE_DEPTH_TEST);
}

void controlEvent(ControlEvent theEvent) {
  if (theEvent.isController()) {
    String name = theEvent.controller().getName();
    // check if theEvent is coming from a boxsize controller
    if (name.startsWith("vert")) {
      // get the id of the controller and map the value
      // to an element inside the boxsize array.
      int id = theEvent.controller().getId() % 100;
      if (name.charAt(name.length() - 1) == editDimension && id >= 0 && id < 12) {
        if (selected >= 0) {
          PVector vert = heptagon.hepts[selected].verts[id];
          PVector origVert = heptagon.hepts[selected].origVerts[id];
          switch (editDimension) {
            case 'x':
              vert.x = origVert.x + theEvent.value();
              break;
            case 'y':
              vert.y = origVert.y + theEvent.value();
              break;
            case 'z':
              vert.z = theEvent.value() * step;
              break;
          }
        }
        else {
          heptagon.setHeptsZ(id, theEvent.value() * step);
        }
      }
    }
    else if (name.equals("extendLegToGlass") || name.equals("legLen") || 
      name.equals("openTop") || name.equals("openBottom")) {
      updateLeg();
    }
    else if (name.equals("step") || name.equals("regularLeg")) {
      updateFromConfig();
    }
    else if (name.equals("insetWOZ")) {
      heptagon.insetWOZ = !regularLeg ? theEvent.value() == 1.0 : false;
    }
  }
}

void keyPressed() {
  codedKeys.keyPressed();
  if (codedKeys.ctrlKey) {
    recordObj = key == 's';
    recordDxf = key == 'd';
    if (recordObj || recordDxf) {
      println("recording started");
    }
  }
  if (key == '0') {
    selected = -1;
  }
  if (key >= '1' && key <= '7') {
    selected = (key - '1');
    updateGui();
  }
  if (key == 'h') {
    drawHalfes = !drawHalfes;
  }
  if (key == 'm') {
    drawMargins = !drawMargins;
  }
  if (key == '.') {
    config = (config + 1) % 5;
    updateFromConfig();
  }
  if (key == ',') {
    config = (config - 1);
    if (config < 0) {
      config = 4;
    }
    updateFromConfig();
  }
  if (key == ' ') {
    showGui = !showGui;
  }
  if (key == 'u') {
    regularLeg = !regularLeg;
    updateFromConfig();
  }
  if (key == 'p') {
    showWall = !showWall;
  }
  if (key == 'd') {
    showFloor = !showFloor;
  }
  if (key == 'g') {
    showGlass = !showGlass;
  }
  if (key == 'f') {
    showFrame = !showFrame;
  }
  if (key == 'e') {
   edgesWidth = edgesWidth == 0 ? (int)cp5.getController("edgesWidth").getValue() : 0;
   updateLeg();
  }
  if (key == 'c') {
    if (pCam.isActive()) {
      pCam.setActive(false);
      //qCam.controllable = true;
    }
    else {
      //qCam.controllable = false;
      pCam.setActive(true);
      pCam.reset();
    }
  }
  if (key == 'r') {
    rotationTarget += 2*PI/2.f;
  }
  if (key == 'q') {
    extendLegToGlass = !extendLegToGlass;
    updateLeg();
  }
  if (key == 'v') {
    showVideo = !showVideo;
    if (showVideo && !showWall) {
      showWall = true;
    }
  }
  if (key == 'i') {
    showInfo = !showInfo;
  }
  if (key == 'n') {
    if (!regularLeg) {
      heptagon.insetWOZ = !heptagon.insetWOZ;
      cp5.getController("insetWOZ").setValue(heptagon.insetWOZ ? 1.0 : 0.0);
    }
  }
  if (key == 'o') {
    heptagon.face.next();
  }
  if (key == 't') {
    if (config == -1) {
      config = 0;
    }
    else {
      config = -1;
    }
    updateFromConfig();
  }
  if (key == 'x' || key == 'y' || key == 'z') {
    editDimension = key;
    
    char dims[] = {'x', 'y', 'z'};
    for (char d : dims) {
      for (int i=0; i<12; i++) {
        cp5.getController("vert"+i+"."+d).setVisible(d == editDimension);
      }
    }
  }
}

void keyReleased() {
  codedKeys.keyReleased();
}

void mousePressed() {
  if (showGui) {
    if ((mouseX >= 10 && mouseX <= 120) || (mouseX >= width - 70)) {
      pCam.setActive(false);
    } else {
      pCam.setActive(true);
    }
  }
}

//void movieEvent(Movie m) {
//  m.read();
//}

void vertex(PVector v) {
  vertex(v.x, v.y, v.z);
}

PShape plane(float w, float h) {
  PShape s = createShape();
  s.beginShape();
  s.vertex(-w/2.f, -h/2.f);
  s.vertex(w/2.f, -h/2.f);
  s.vertex(w/2.f, h/2.f);
  s.vertex(-w/2.f, h/2.f);
  s.endShape(CLOSE);
  return s;
}

PShape heptagon(float radius, boolean addClosingVertex) {
  PShape s = createShape();
  s.beginShape();
  for (int i=0; i<7; i++) {
    PVector p = pointOnEllipse(2*PI/7.f*i, radius, radius);
    s.vertex(p.x, p.y);
  }
  if (addClosingVertex) {
    PVector p = pointOnEllipse(0, radius, radius);
    s.vertex(p.x, p.y);
  }
  s.endShape(CLOSE);
  return s;
}

PShape heptagon(float radius) {
  return heptagon(radius, false);
}

PShape subtract(PShape outer, PShape inner, boolean useContour) {
  PShape s = createShape();
  PVector v;
  s.beginShape();
  for (int i=0; i<outer.getVertexCount(); i++) {
    v = outer.getVertex(i);
    s.vertex(v.x, v.y);
  }
  if (useContour) {
    s.beginContour();
    for (int i=0; i<inner.getVertexCount(); i++) {
      v = inner.getVertex(i);
      s.vertex(v.x, v.y);
    }
    s.endContour();
  }
  else {
    v = outer.getVertex(0);
    s.vertex(v.x, v.y);
    v = inner.getVertex(0);
    s.vertex(v.x, v.y);
    for (int i=inner.getVertexCount()-1; i>=0; i--) {
      v = inner.getVertex(i);
      s.vertex(v.x, v.y);
    }
  }
  s.endShape(CLOSE);
  return s;
}

PShape subtract(PShape a, PShape b) {
  return subtract(a, b, true);
}

PVector pointOnEllipse(float rad, float w, float h) {
  return new PVector(cos(rad) * w / 2.f, sin(rad) * h/2.f);
}

PVector intersection(PVector a, PVector b, PVector c, PVector d, boolean checkLengths) {
  return (new PLine(a, b)).intersection(new PLine(c, d), checkLengths);
}

PVector intersection(PVector a, PVector b, PVector c, PVector d) {
  return intersection(a, b, c, d, true);
}

PVector centroid(PVector ... verts) {
  PVector centroid = new PVector();
  int numVerts = 0;
  for (PVector v : verts) {
    centroid.add(v);
    numVerts++;
  }
  return centroid.div((float) numVerts);
}

// http://mathworld.wolfram.com/Incenter.html
PVector incircle(PVector a, PVector b, PVector c) {
    float la = PVector.sub(b, c).mag();
    float lb = PVector.sub(c, a).mag();
    float lc = PVector.sub(a, b).mag();
    float labc = la + lb + lc;
    return PVector.div(PVector.add(PVector.add(PVector.mult(a, la), PVector.mult(b, lb)), PVector.mult(c, lc)), labc);
}

// http://mathworld.wolfram.com/Inradius.html
float inradius(PVector a, PVector b, PVector c) {
  float la = PVector.sub(b, c).mag();
  float lb = PVector.sub(c, a).mag();
  float lc = PVector.sub(a, b).mag();
  float labc = la + lb + lc;
  return 0.5 * sqrt((-la + lb + lc) * (la - lb + lc) * (la + lb - lc) / labc);
}

PVector inset(PVector a, PVector b, PVector c, float value) {
  PVector q = incircle(a, b, c);
  PVector qa = PVector.sub(q, a);
  float r = inradius(a, b, c);
  return PVector.add(a, PVector.mult(qa, value / r));
}

PVector inset(PVector a, PVector b, PVector c, PVector value) {
  if (value.x == value.y) {
    return inset(a, b, c, value.x);
  }
  PVector ba = PVector.sub(b, a);
  PVector ca = PVector.sub(c, a);
  float angle = PVector.angleBetween(ba, ca);
  return PVector.add(a, ba.copy().limit(value.x / sin(angle)))
    .add(ca.copy().limit(value.y / sin(angle)));
}

PVector insetTrig(PVector a, PVector b, PVector c, float value) {
  float aspect = 1.0;
  PVector cb = PVector.sub(c, b);
  PVector ba = PVector.sub(b, a);
  PVector ca = PVector.sub(c, a);
  float lba = ba.mag();
  float lca = ca.mag();
  float ratio = lba / ( lba + lca );
  PVector d = PVector.add(b, PVector.mult(cb, ratio));
  PVector ad = PVector.sub(d, a);
  PVector bisect = ad.copy().normalize();

  float theta = acos( PVector.dot(ba, ca) / (lba * lca) ) / 2.0;
  float ae = value / (sin(theta) * aspect);
  return PVector.add(a, new PVector(bisect.x * ae, bisect.y * ae, ae / ad.mag() * (d.z - a.z)));
}

PVector insetMult(PVector a, PVector b, PVector c, PVector value) {
  return PVector.add(a, 
    PVector.add(
      PVector.sub(b, a).limit(value.x), 
      PVector.sub(c, a).limit(value.y)
    )
  );
}

PVector[] insetTriangle(PVector a, PVector b, PVector c, float value) {
  PVector ia = inset(a, b, c, value);
  PVector ib = inset(b, c, a, value);
  PVector ic = inset(c, a, b, value);        
  PVector[] triangle = {ia, ib, ic};
  return triangle;
}

PVector[] insetTriangle(PVector a, PVector b, PVector c, PVector va, PVector vb, PVector vc) {
  PVector ia = va.x == va.y ? inset(a, b, c, va.x) : inset(a, b, c, va);
  PVector ib = vb.x == vb.y ? inset(b, c, a, vb.x) : inset(b, c, a, vb);
  PVector ic = vc.x == vc.y ? inset(c, a, b, vc.x) : inset(c, a, b, vc);        
  PVector[] triangle = {ia, ib, ic};
  return triangle;
}

PVector[] insetQuad(PVector a, PVector b, PVector c, PVector d, float value) {
  PVector ia = inset(a, b, d, value);
  PVector ib = inset(b, c, a, value);
  PVector ic = inset(c, d, b, value);  
  PVector id = inset(d, a, c, value);  
  PVector[] quad = {ia, ib, ic, id};
  return quad;
}

PVector[] insetQuad(PVector a, PVector b, PVector c, PVector d, PVector va, PVector vb, PVector vc, PVector vd) {
  PVector ia = va.x == va.y ? inset(a, b, d, va.x) : inset(a, b, d, va);
  PVector ib = vb.x == vb.y ? inset(b, c, a, vb.x) : inset(b, c, a, vb);
  PVector ic = vc.x == vc.y ? inset(c, d, b, vc.x) : inset(c, d, b, vc);
  PVector id = vd.x == vd.y ? inset(d, a, c, vd.x) : inset(d, a, c, vd);
  PVector[] quad = {ia, ib, ic, id};
  return quad;
}

PVector insetCentroid(PVector vert, PVector centroid, float value) {
  return PVector.add(vert, PVector.sub(centroid, vert).normalize().mult(value));
}

PVector surfaceNormal(PVector a, PVector b, PVector c) {
  PVector U = PVector.sub(b, a);
  PVector V = PVector.sub(c, a);

  return new PVector(
    U.y * V.z - U.z * V.y, 
    U.z * V.x - U.x * V.z, 
    U.x * V.y - U.y * V.x
  ).normalize();
}

float heading3D(PVector v) {
  float rho = sqrt(pow(v.x, 2) + pow(v.y, 2) + pow(v.z, 2));
  return acos(v.z/rho);
}

int signum(float f) {
  if (f > 0) return 1;
  if (f < 0) return -1;
  return 0;
} 
