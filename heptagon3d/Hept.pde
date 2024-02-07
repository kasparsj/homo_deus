class Hept {
  Heptagon gon;
  int index;
  PVector[] verts;
  PVector[] origVerts;
  PVector[][] vertsInset;
  ArrayList<PVector[]> faces;
  float length1;
  float length2;
  
  Hept(Heptagon gon, int index) {
    this.gon = gon;
    this.index = index;
    verts = new PVector[12];
    origVerts = new PVector[12];
    reset();
  }
  
  void reset() {
    float size = gon.size;
    PVector outer = pointOnEllipse(2*PI/7.f*index, size, size);
    PVector outerNext1 = pointOnEllipse(2*PI/7.f*(index+1), size, size);
    PVector outerNext2 = pointOnEllipse(2*PI/7.f*(index+2), size, size);
    PVector outerNext3 = pointOnEllipse(2*PI/7.f*(index+3), size, size);
    PVector outerPrev1 = pointOnEllipse(2*PI/7.f*(index-1), size, size);
    PVector outerPrev2 = pointOnEllipse(2*PI/7.f*(index-2), size, size);
    PVector outerHalfEllipse = pointOnEllipse(2*PI/7.f*(index+0.5), size, size);
    PVector outerHalf = intersection(outer, outerNext1, new PVector(0, 0), outerHalfEllipse);
    PVector inner1 = intersection(outer, outerNext2, outerHalfEllipse, new PVector(0, 0));
    PVector inner2Left = intersection(outer, outerNext3, outerNext1, outerPrev1);
    PVector inner2Right = intersection(outerNext1, outerPrev2, outer, outerNext2);
    PVector inner3 = intersection(outer, outerNext3, outerHalfEllipse, new PVector(0, 0));
    PVector inner4 = intersection(outerPrev1, outerNext2, outerHalfEllipse, new PVector(0, 0));
    PVector side1Left = intersection(outer, new PVector(0, 0), outerNext1, outerPrev1);
    PVector side1Right = intersection(outer, outerNext2, outerNext1, new PVector(0, 0));
    PVector side2Left = intersection(outer, new PVector(0, 0), outerNext1, outerPrev2);
    PVector side2Right = intersection(outerNext1, new PVector(0, 0), outer, outerNext3);
    
    verts[0] = outer;
    verts[1] = outerHalf;
    verts[2] = outerNext1;
    verts[3] = inner1;
    verts[4] = inner2Left;
    verts[5] = inner2Right;
    verts[6] = side1Left;
    verts[7] = side1Right;
    verts[8] = inner3;
    verts[9] = side2Left;
    verts[10] = inner4;
    verts[11] = side2Right;
    
    for (int i=0; i<verts.length; i++) {
      origVerts[i] = verts[i].copy();
    }
  }
  
  void update() {
    if (gon.insetWOZ) {
      vertsInset = insetVerts(copyVertsWOZ());
      applyZToInsetV(vertsInset);
    }
    else {
      vertsInset = insetVerts(verts);
    }
    updateFaces();
  }
  
  void updateFaces() {
    length1 = 0;
    length2 = 0;
    int i = 0;
    faces = new ArrayList<PVector[]>();
    if (gon.inset > 0) {
      if (gon.halfes) {
        // 1
        faces.add(vertsInset[i]);
        // 2
        faces.add(vertsInset[++i]);
      }
      else {
        // 1
        faces.add(vertsInset[i]);
      }
      // 3 / 2
      faces.add(vertsInset[++i]);
      // 4 / 3
      faces.add(vertsInset[++i]);
      if (gon.inset != 0) {
        // 1 x 3
        faces.add(new PVector[]{vertsInset[0][0], vertsInset[0][1], vertsInset[1][2], vertsInset[1][1]});
        length2 += PVector.dist(PVector.add(vertsInset[0][0], vertsInset[1][1]).div(2), PVector.add(vertsInset[0][1], vertsInset[1][2]).div(2));  
        // 1 x 4
        faces.add(new PVector[]{vertsInset[0][2], vertsInset[0][0], vertsInset[2][0], vertsInset[2][2]});
        length2 += PVector.dist(PVector.add(vertsInset[0][2], vertsInset[2][2]).div(2), PVector.add(vertsInset[0][0], vertsInset[2][0]).div(2));
      }
      if (gon.halfes) {
        faces.add(vertsInset[++i]);
        // 6
        faces.add(vertsInset[++i]);
      }
      else {
        // 5 + 6 / 4
        faces.add(vertsInset[++i]);
        if (gon.inset != 0) {
          // 5+6 x 3
          faces.add(new PVector[]{vertsInset[3][0], vertsInset[3][1], vertsInset[1][1], vertsInset[1][0]});
          length2 += PVector.dist(PVector.add(vertsInset[3][0], vertsInset[1][0]).div(2), PVector.add(vertsInset[3][1], vertsInset[1][1]).div(2));
          // 5+6 x 4
          faces.add(new PVector[]{vertsInset[3][1], vertsInset[3][2], vertsInset[2][1], vertsInset[2][0]});
          length2 += PVector.dist(PVector.add(vertsInset[3][1], vertsInset[2][0]).div(2), PVector.add(vertsInset[3][2], vertsInset[2][1]).div(2));
        }
      }
      // 7 / 5
      faces.add(vertsInset[++i]);
      // 8 / 6
      faces.add(vertsInset[++i]);
      if (gon.inset != 0) {
        // 7 x 3
        faces.add(new PVector[]{vertsInset[i-1][0], vertsInset[i-1][2], vertsInset[1][2], vertsInset[1][0]});
        length1 += PVector.dist(PVector.add(vertsInset[i-1][0], vertsInset[1][0]).div(2), PVector.add(vertsInset[i-1][2], vertsInset[1][2]).div(2));
        // 8 x 4
        faces.add(new PVector[]{vertsInset[i][0], vertsInset[i][2], vertsInset[2][1], vertsInset[2][2]});
        length1 += PVector.dist(PVector.add(vertsInset[i][0], vertsInset[2][2]).div(2), PVector.add(vertsInset[i][2], vertsInset[2][1]).div(2));
        if (gon.margins) {
          // margin / holder
          faces.add(new PVector[]{vertsInset[0][1], vertsInset[0][2], verts[2], verts[0]});
        }
      }
      // 9 / 7
      faces.add(vertsInset[++i]);
      // 10 / 8
      faces.add(vertsInset[++i]);
      if (gon.inset != 0) {
        // 7 x 9
        faces.add(new PVector[]{vertsInset[4][0], vertsInset[4][1], vertsInset[6][2], vertsInset[6][3]});
        length2 += PVector.dist(PVector.add(vertsInset[4][0], vertsInset[6][3]).div(2), PVector.add(vertsInset[4][1], vertsInset[6][2]).div(2));
        // 5+6 x 9
        faces.add(new PVector[]{vertsInset[3][0], vertsInset[3][3], vertsInset[6][0], vertsInset[6][3]});
        length1 += PVector.dist(PVector.add(vertsInset[3][0], vertsInset[6][3]).div(2), PVector.add(vertsInset[3][3], vertsInset[6][0]).div(2));
        // 8 x 10 
        faces.add(new PVector[]{vertsInset[5][1], vertsInset[5][2], vertsInset[7][1], vertsInset[7][2]});
        length2 += PVector.dist(PVector.add(vertsInset[5][1], vertsInset[7][2]).div(2), PVector.add(vertsInset[5][2], vertsInset[7][1]).div(2));
        // 5+6 x 10
        faces.add(new PVector[]{vertsInset[3][2], vertsInset[3][3], vertsInset[7][0], vertsInset[7][1]});
        length1 += PVector.dist(PVector.add(vertsInset[3][2], vertsInset[7][1]).div(2), PVector.add(vertsInset[3][3], vertsInset[7][0]).div(2));
      }
      if (gon.halfes) {
        // 11
        faces.add(vertsInset[++i]);
        // 12
        faces.add(vertsInset[++i]);
      }
      else {
        // 11 + 12
        faces.add(vertsInset[++i]);
        if (gon.inset != 0) {
          // 11+12 x 9 
          faces.add(new PVector[]{vertsInset[8][1], vertsInset[8][2], vertsInset[6][1], vertsInset[6][0]});
          length1 += PVector.dist(PVector.add(vertsInset[8][1], vertsInset[6][0]).div(2), PVector.add(vertsInset[8][2], vertsInset[6][1]).div(2));
          // 11+12 x 10 
          faces.add(new PVector[]{vertsInset[8][0], vertsInset[8][1], vertsInset[7][0], vertsInset[7][3]});
          length1 += PVector.dist(PVector.add(vertsInset[8][0], vertsInset[7][3]).div(2), PVector.add(vertsInset[8][1], vertsInset[7][0]).div(2));
          // square 1+2 x 3 x 5+6 x 4 
          faces.add(new PVector[]{vertsInset[0][0], vertsInset[1][1], vertsInset[3][1], vertsInset[2][0]});
          length2 += PVector.dist(PVector.add(vertsInset[0][0], vertsInset[1][1]).div(2), PVector.add(vertsInset[3][1], vertsInset[2][0]).div(2)) * 2;
          // square 3 x 7 x 9 x 5+6
          faces.add(new PVector[]{vertsInset[1][0], vertsInset[4][0], vertsInset[6][3], vertsInset[3][0]});
          length1 += PVector.dist(PVector.add(vertsInset[1][0], vertsInset[4][0]).div(2), PVector.add(vertsInset[6][3], vertsInset[3][0]).div(2));
          length2 += PVector.dist(PVector.add(vertsInset[1][0], vertsInset[3][0]).div(2), PVector.add(vertsInset[4][0], vertsInset[6][3]).div(2));
          // square 4 x 8 x 10 x 5+6  
          faces.add(new PVector[]{vertsInset[2][1], vertsInset[5][2], vertsInset[7][1], vertsInset[3][2]});
          length1 += PVector.dist(PVector.add(vertsInset[2][1], vertsInset[5][2]).div(2), PVector.add(vertsInset[7][1], vertsInset[3][2]).div(2));
          length2 += PVector.dist(PVector.add(vertsInset[2][1], vertsInset[3][2]).div(2), PVector.add(vertsInset[5][2], vertsInset[7][1]).div(2));
          // square 5+6 x 10 x 11+12 x 9  
          faces.add(new PVector[]{vertsInset[3][3], vertsInset[7][0], vertsInset[8][1], vertsInset[6][0]});
          length1 += PVector.dist(PVector.add(vertsInset[3][3], vertsInset[7][0]).div(2), PVector.add(vertsInset[8][1], vertsInset[6][0]).div(2)) * 2;
        }
      }
    }
    else {
      if (gon.halfes) {
        faces.add(new PVector[]{verts[0], verts[1], verts[3]}); // 1
        faces.add(new PVector[]{verts[1], verts[2], verts[3]}); // 2
      }
      else {
        faces.add(new PVector[]{verts[3], verts[0], verts[2]}); // 1
      }
      // 3 / 2
      faces.add(new PVector[]{verts[4], verts[3], verts[0]});
      // 4 / 3
      faces.add(new PVector[]{verts[3], verts[5], verts[2]});
      if (gon.halfes) {
        // 5
        faces.add(new PVector[]{verts[3], verts[8], verts[4]});
        // 6
        faces.add(new PVector[]{verts[3], verts[8], verts[5]});
      }
      else {
        // 5 + 6 / 4
        faces.add(new PVector[]{verts[4], verts[3], verts[5], verts[8]});
      }
      // 7 / 5
      faces.add(new PVector[]{verts[4], verts[6], verts[0]});
      // 8 / 6
      faces.add(new PVector[]{verts[2], verts[7], verts[5]});
      // 9 / 7
      faces.add(new PVector[]{verts[8], verts[9], verts[6], verts[4]});
      // 10 / 8
      faces.add(new PVector[]{verts[8], verts[5], verts[7], verts[11]});    
      if (gon.halfes) {
        // 11
        faces.add(new PVector[]{verts[8], verts[10], verts[9]});
        // 12
        faces.add(new PVector[]{verts[8], verts[10], verts[11]});
      }
      else {
        // 11 + 12
        //vertsInset[++i] = gon._insetTriangle(v[11], v[8], v[9], new PVector(0.5, 1), new PVector(1, 1), new PVector(1, 0.5));
        faces.add(new PVector[]{verts[11], verts[8], verts[9]});
      }
    }
  }
  
  PVector[] copyVertsWOZ() {
    PVector[] vertsCopy = new PVector[verts.length];
    for (int i=0; i<vertsCopy.length; i++) {
      vertsCopy[i] = verts[i].copy();
      vertsCopy[i].z = 0;
    }
    return vertsCopy;
  }
  
  PVector[][] insetVerts(PVector[] v) {
    PVector[][] vertsInset = new PVector[gon.halfes ? 12 : 9][];
    int i = 0;
    if (gon.halfes) {
      // 1
      vertsInset[i] = gon._insetTriangle(v[0], v[1], v[3]);
      // 2
      vertsInset[++i] = gon._insetTriangle(v[1], v[2], v[3]);
    }
    else {
      // 1
      vertsInset[i] = gon._insetTriangle(v[3], v[0], v[2]);
    }
    // 3 / 2
    vertsInset[++i] = gon._insetTriangle(v[4], v[3], v[0]);
    // 4 / 3
    vertsInset[++i] = gon._insetTriangle(v[3], v[5], v[2]);
    vertsInset[i][2] = inset(verts[2], verts[3], verts[5], gon.inset);
    if (gon.halfes) {
      // 5
      vertsInset[++i] = gon._insetTriangle(v[3], v[8], v[4]);
      // 6
      vertsInset[++i] = gon._insetTriangle(v[3], v[8], v[5]);
    }
    else {
      // 5 + 6 / 4
      vertsInset[++i] = gon._insetQuad(v[4], v[3], v[5], v[8]);
    }
    // 7 / 5
    vertsInset[++i] = gon._insetTriangle(v[4], v[6], v[0], new PVector(1, 1), new PVector(1, 0), new PVector(0, 1));
    // 8 / 6
    vertsInset[++i] = gon._insetTriangle(v[2], v[7], v[5], new PVector(1, 0), new PVector(0, 1), new PVector(1, 1));
    // 9 / 7
    vertsInset[++i] = gon._insetQuad(v[8], v[9], v[6], v[4], 
      new PVector(1, 1), new PVector(1, 0), new PVector(0, 1), new PVector(1, 1));
    // 10 / 8
    vertsInset[++i] = gon._insetQuad(v[8], v[5], v[7], v[11], 
      new PVector(1, 1), new PVector(1, 1), new PVector(1, 0), new PVector(0, 1));    
    if (gon.halfes) {
      // 11
      vertsInset[++i] = gon._insetTriangle(v[8], v[10], v[9]);
      // 12
      vertsInset[++i] = gon._insetTriangle(v[8], v[10], v[11]);
    }
    else {
      // 11 + 12
      //vertsInset[++i] = gon._insetTriangle(v[11], v[8], v[9], new PVector(0.5, 1), new PVector(1, 1), new PVector(1, 0.5));
      vertsInset[++i] = gon._insetTriangle(v[11], v[8], v[9]);
    }
    return vertsInset;
  }
  
  void applyZToInsetV(PVector[][] insetVerts) {
    int i = 0;
    if (gon.halfes) {
      // 1
      insetVerts[i][0].z = verts[0].z;
      insetVerts[i][1].z = verts[1].z;
      insetVerts[i][2].z = verts[3].z;
      // 2
      insetVerts[++i][0].z = verts[1].z;
      insetVerts[i][1].z = verts[2].z;
      insetVerts[i][2].z = verts[3].z;
    }
    else {
      // 1 + 2 / 1
      insetVerts[i][0].z = verts[3].z;
      insetVerts[i][1].z = -gon.inset; //verts[0].z;
      insetVerts[i][2].z = -gon.inset; //verts[2].z;
    }
    // 3 / 2
    insetVerts[++i][0].z = verts[4].z;
    insetVerts[i][1].z = verts[3].z;
    insetVerts[i][2].z = -gon.inset; //verts[0].z;
    // 4 / 3
    insetVerts[++i][0].z = verts[3].z;
    insetVerts[i][1].z = verts[5].z;
    insetVerts[i][2].z = -gon.inset; //verts[2].z;
    if (gon.halfes) {
      // 5
      insetVerts[++i][0].z = verts[3].z;
      insetVerts[i][1].z = verts[8].z;
      insetVerts[i][2].z = verts[4].z;
      // 6
      insetVerts[++i][0].z = verts[3].z;
      insetVerts[i][1].z = verts[8].z;
      insetVerts[i][2].z = verts[5].z;
    }
    else {
      // 5 + 6 / 4
      insetVerts[++i][0].z = verts[4].z;
      insetVerts[i][1].z = verts[3].z;
      insetVerts[i][2].z = verts[5].z;
      insetVerts[i][3].z = verts[8].z;
    }
    // 7 / 5
    insetVerts[++i][0].z = verts[4].z;
    insetVerts[i][1].z = verts[6].z;
    insetVerts[i][2].z = -gon.inset; //verts[0].z;
    // 8 / 6
    insetVerts[++i][0].z = -gon.inset; //verts[2].z;
    insetVerts[i][1].z = verts[7].z;
    insetVerts[i][2].z = verts[5].z;
    // 9 / 7
    insetVerts[++i][0].z = verts[8].z;
    insetVerts[i][1].z = verts[9].z;
    insetVerts[i][2].z = verts[6].z;
    insetVerts[i][3].z = verts[4].z;
    // 10 / 8
    insetVerts[++i][0].z = verts[8].z;
    insetVerts[i][1].z = verts[5].z;
    insetVerts[i][2].z = verts[7].z;
    insetVerts[i][3].z = verts[11].z;
    if (gon.halfes) {
      // 11
      insetVerts[++i][0].z = verts[8].z;
      insetVerts[i][1].z = verts[10].z;
      insetVerts[i][2].z = verts[9].z;
      // 12
      insetVerts[++i][0].z = verts[8].z;
      insetVerts[i][1].z = verts[10].z;
      insetVerts[i][2].z = verts[11].z;
    }
    else {
      // 11 + 12 / 9
      insetVerts[++i][0].z = verts[11].z;
      insetVerts[i][1].z = verts[8].z;
      insetVerts[i][2].z = verts[9].z;
    }
  }
  
  void draw() {
    for (int i=0; i<faces.size(); i++) {
      PVector[] face = faces.get(i);
      if (face.length == 3) {
        beginShape(TRIANGLES);        
        vertex(face[0]);
        vertex(face[1]);
        vertex(face[2]);
        endShape();
      }
      else {
        beginShape(QUADS);
        vertex(face[0]);
        vertex(face[1]);
        vertex(face[2]);
        vertex(face[3]);
        endShape();
      }
    }
  }
}
