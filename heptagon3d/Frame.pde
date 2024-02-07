class Frame {
  Heptagon gon;
  float depth; // 260
  float rotation = 0; // 13
  float sizeMM = 15;
  float k;
  ArrayList<PVector[]> boxes;
  float[] lengthsMM;
  
  Frame(Heptagon gon) {
    this.gon = gon;
    this.lengthsMM = new float[7];
  }
  
  void update() {
    k = edgesWidth / 8.f;
  }
  
  void draw() {
    drawFrame(sizeMM * k);
    pushMatrix();
    translate(0, 0, -depth * k);
    rotateY(radians(rotation));
    drawFrame(sizeMM * k);
    popMatrix();
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
    for (int i=0; i<7; i++) {
      //PVector outer = gon.hepts[i].verts[0];
      //PVector outerNext1 = gon.hepts[i].verts[2];
      PVector outer = pointOnEllipse(2*PI/7.f*i, gon.size+side, gon.size+side);
      PVector outerNext = pointOnEllipse(2*PI/7.f*(i+1), gon.size+side, gon.size+side);
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
    for (int i=0; i<7; i++) {
      //PVector outer = gon.hepts[i].verts[0];
      PVector outer = pointOnEllipse(2*PI/7.f*i, gon.size, gon.size);
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
      //rotate(heading3D(PVector.sub(outer, outerDown)));
      box(side, side, len);
      lengthsMM[i] = len / k;
      popMatrix();
    }
  }
}
