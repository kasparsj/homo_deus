class Frame {
  Heptagon gon;
  float depth; // 260
  float rotation = 0; // 13
  float sizeMM = 15;
  float k;
  boolean drawOutside = false;
  boolean drawBottom = true;
  boolean offsetConn = true;
  ArrayList<PVector[]> boxes;
  float[] lengthsMM;
  
  Frame(Heptagon gon) {
    this.gon = gon;
    this.lengthsMM = new float[7];
  }
  
  void update() {
    if (edgesWidth > 0) {
      k =  edgesWidth / 8.f;
      drawOutside = false;
    }
    else {
      k =  1;
      drawOutside = true;
    }
  }
  
  void draw() {
    drawFrame(sizeMM * k);
    if (drawBottom) {
      pushMatrix();
      translate(0, 0, -depth * k);
      rotateY(radians(rotation));
      drawFrame(sizeMM * k);
      popMatrix();
    }
    drawConnections(sizeMM * k);
    //pushMatrix();
    //pushStyle();
    //fill(255, 0, 0, 255/4);
    //translate(0, 0, (-depth-sizeMM) * k);
    //shape(heptagon(diam, true));
    //popStyle();
    //popMatrix();
  }
  
  void drawFrame(float side) {
    float offset = drawOutside ? side*2 : side;
    for (int i=0; i<7; i++) {
      //PVector outer = gon.hepts[i].verts[0];
      //PVector outerNext1 = gon.hepts[i].verts[2];
      PVector outer = pointOnEllipse(2*PI/7.f*i, gon.size+offset, gon.size+offset);
      PVector outerNext = pointOnEllipse(2*PI/7.f*(i+1), gon.size+offset, gon.size+offset);
      float w = PVector.sub(outer, outerNext).mag();
      pushMatrix();
      translate(outer.x, outer.y, outer.z - side/2);
      rotate(TWO_PI/7.f*i+(TWO_PI-5*PI/7.f)/2.f);
      translate(w/2, side/2, 0);
      box(w, side, side);
      popMatrix();
    }
  }
  
  void drawConnections(float side) {
    float offset = drawOutside ? side : 0;
    for (int i=0; i<7; i++) {
      //PVector outer = gon.hepts[i].verts[0];
      PVector outer = pointOnEllipse(2*PI/7.f*i, gon.size+offset, gon.size+offset);
      Vec3D temp = new Vec3D(outer.x, outer.y, outer.z - depth * k);
      temp.rotateY(radians(-rotation));
      PVector outerDown = new PVector(temp.x, temp.y, temp.z);
      float d = PVector.sub(outer, outerDown).mag();
      float len = d - side;
      //pushMatrix();
      //translate(outerDown.x, outerDown.y, outerDown.z);
      //box(side);
      //popMatrix();
      pushMatrix();
      translate(outer.x, outer.y, outer.z - d/2.f - side/2.f);
      if (offsetConn) {
        rotate(TWO_PI/7.f*i+(TWO_PI-5*PI/7.f)/2.f);
        translate(side/2, 0, 0);
      }
      box(side, side, len);
      lengthsMM[i] = len / k;
      popMatrix();
    }
  }
}
