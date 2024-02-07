class Tunnel {
  float diam;
  float length;
  int numSides;
  PVector rotation;
  PVector dir;
  PVector[][] verts;
  
  private float _diam;
  
  Tunnel(float diam, float length, int numSides, PVector rotation) {
    this.diam = diam;
    this.length = length;
    this.numSides = numSides;
    this.rotation = rotation;
    verts = new PVector[numSides][4];
    reset();
  }
  
  Tunnel(float diam, float length, int numSides) {
    this(diam, length, numSides, null);
  }
  
  void reset() {
    dir = new PVector(0, 0, 1);
    _diam = 0;
  }
  
  void update() {
    if (diam != _diam) {
      for (int i=0; i<numSides; i++) {
        PVector p1 = pointOnEllipse(2*PI/numSides*i, diam, diam);
        PVector p2 = pointOnEllipse(2*PI/numSides*(i+1), diam, diam);
        updateVerts(i, p1, p2);
      }
      _diam = diam;
    }
  }
  
  void updateVerts(int i, PVector p1, PVector p2) {
    PVector v1 = p1.copy();
    PVector v2 = p2.copy();
    PVector v3 = v2.copy().add(PVector.mult(dir, length));
    PVector v4 = v1.copy().add(PVector.mult(dir, length));
    verts[i][0] = v1;
    verts[i][1] = v2;
    verts[i][2] = v3;
    verts[i][3] = v4;
  }
  
  void draw() {
    pushMatrix();
    if (rotation != null) {
      rotateY(heading3D(rotation));
    }
    for (int i=0; i<numSides; i++) {
      beginShape(QUADS);
      vertex(verts[i][0].x, verts[i][0].y, verts[i][0].z);
      vertex(verts[i][1].x, verts[i][1].y, verts[i][1].z);
      vertex(verts[i][2].x, verts[i][2].y, verts[i][2].z);
      vertex(verts[i][3].x, verts[i][3].y, verts[i][3].z);
      endShape();
    }
    popMatrix();
  }
  
  PShape getGlassCutOut(boolean openTop, boolean openBottom) {
    PShape innerShape = createShape();
    innerShape.beginShape();
    if (openBottom) {
      PVector p = pointOnEllipse(0, diam, diam);
      innerShape.vertex(p.x, p.y);
    }
    for (int i=0; i<leg.numSides; i++) {
      innerShape.vertex(leg.verts[i][0].x, leg.verts[i][0].y);
    }
    innerShape.vertex(leg.verts[6][1].x, leg.verts[6][1].y);
    if (openBottom) {
      PVector p = pointOnEllipse(0, diam, diam);
      innerShape.vertex(p.x, p.y);
    }
    innerShape.endShape();
    return innerShape;
  }
}
