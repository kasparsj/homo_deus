class Heptagon {
  float size;
  boolean halfes;
  boolean margins;
  float inset = 0;
  boolean insetWOZ;
  Hept[] hepts;
  Face face;
  Frame frame;
  
  private float _size;
  
  Heptagon(float size) {
    this.size = size;
    hepts = new Hept[7];
    face = new Face(this);
    frame = new Frame(this);
  }
  
  void updateSize() {
    if (size != _size) {
      for (int i=0; i<7; i++) {
        hepts[i] = new Hept(this, i);
      }
      _size = size;
    }
  }
  
  void update() {
    for (int i=0; i<7; i++) {
      hepts[i].update();
    };
    face.update();
    frame.update();
  }
  
  void setHeptsX(int v, float val) {
    for (int i=0; i<7; i++) {
      hepts[i].verts[v].x = val;
    }
  }
  
  void setHeptsY(int v, float val) {
    for (int i=0; i<7; i++) {
      hepts[i].verts[v].y = val;
    }
  }
  
  void setHeptsZ(int v, float val) {
    for (int i=0; i<7; i++) {
      hepts[i].verts[v].z = val;
    }
  }
  
  void draw() {
    for (int i=0; i<7; i++) {
      hepts[i].draw();
    }
  }
  
  float getLength1() {
    float len = 0;
    for (int i=0; i<7; i++) {
      len += hepts[i].length1;
    }
    return len;
  }
  
  float getLength2() {
    float len = 0;
    for (int i=0; i<7; i++) {
      len += hepts[i].length2;
    }
    return len;
  }
  
  PVector[] _insetTriangle(PVector a, PVector b, PVector c) {
    return insetTriangle(a, b, c, inset);
  }
  
  PVector[] _insetTriangle(PVector a, PVector b, PVector c, PVector ia, PVector ib, PVector ic) {
    return insetTriangle(a, b, c, 
      PVector.mult(ia, inset), 
      PVector.mult(ib, inset), 
      PVector.mult(ic, inset));
  }
  
  PVector[] _insetQuad(PVector a, PVector b, PVector c, PVector d) {
    return insetQuad(a, b, c, d, inset);
  }
  
  PVector[] _insetQuad(PVector a, PVector b, PVector c, PVector d, 
    PVector ia, PVector ib, PVector ic, PVector id) {
    return insetQuad(a, b, c, d, 
      PVector.mult(ia, inset), 
      PVector.mult(ib, inset), 
      PVector.mult(ic, inset), 
      PVector.mult(id, inset)); 
  }
}
