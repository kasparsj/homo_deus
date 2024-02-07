class Face {
  
  static final int TYPE_CONE = 0;
  static final int TYPE_ROBO = 1;
  static final int TYPE_PYRAMIDS = 2;
  static final int TYPE_HOLLOW = 3;
  
  Heptagon gon;
  int type;
  float z = 0;
  ArrayList<PVector[]> faces;
  
  Face(Heptagon gon) {
    this.gon = gon;
    type = TYPE_ROBO;
    faces = new ArrayList<PVector[]>();
  }
  
  void next() {
    type = (type + 1) % 4;
  }
  
  void update() {
    faces.clear();
    switch (type) {
      case TYPE_CONE:
        updateTypeCone();
        break;
      case TYPE_ROBO:
        updateTypeRobo();
        break;
      case TYPE_PYRAMIDS:
        updateTypePyramids();
        break;
      
    }
  }
  
  void updateTypeCone() {
    for (int i=0; i<7; i++) {
      PVector center1 = pointOnEllipse(TWO_PI/7.f*i, eyesR, eyesR);
      PVector center2 = pointOnEllipse(TWO_PI/7.f*(i+1), eyesR, eyesR);
      PVector outer1 = gon.hepts[i].verts[9].copy();
      PVector outer2 = gon.hepts[i].verts[11].copy();
      if (gon.insetWOZ) {
        outer1.z = 0;
        outer2.z = 0;
      }
      PVector[] quadInset = gon._insetQuad(center2, center1, 
        outer1, outer2,
        new PVector(0, 0), new PVector(0, 0), new PVector(0, 1), new PVector(1, 0));
      if (gon.insetWOZ) {
        quadInset[0].z = 0;
        quadInset[1].z = 0;
        quadInset[2].z = gon.hepts[i].verts[9].z;
        quadInset[3].z = gon.hepts[i].verts[11].z;
      }
      faces.add(quadInset);
      faces.add(new PVector[]{gon.hepts[i].vertsInset[8][0], gon.hepts[i].vertsInset[8][2], 
        quadInset[2], 
        quadInset[3]});
    }
  }
  
  void updateTypeRobo() {
    PVector center1, center2, outer1, outer2;
    for (int i=0; i<7; i++) {
      center1 = pointOnEllipse(TWO_PI/7.f*i, eyesR, eyesR);
      center2 = pointOnEllipse(TWO_PI/7.f*(i+1), eyesR, eyesR);
      outer1 = gon.hepts[i].verts[9].copy();
      outer2 = gon.hepts[i].verts[11].copy();
      if (i < 3) {
        center1.add(new PVector(0, eyesD, 0));
        center2.add(new PVector(0, eyesD, 0));
      }
      else if (i == 3) {
        center1.add(new PVector(0, eyesD, 0));
        center2.sub(new PVector(0, eyesD, 0));
      }
      else if (i < 7) {
        center1.sub(new PVector(0, eyesD, 0));
        center2.sub(new PVector(0, eyesD, 0));
      }
      if (gon.insetWOZ) {
        outer1.z = 0;
        outer2.z = 0;
      }
      PVector[] quadInset = gon._insetQuad(center2, center1, 
        outer1, outer2,
        new PVector(0, 0), new PVector(0, 0), new PVector(0, 1), new PVector(1, 0));
      if (gon.insetWOZ) {
        quadInset[0].z = 0;
        quadInset[1].z = 0;
        quadInset[2].z = gon.hepts[i].verts[9].z;
        quadInset[3].z = gon.hepts[i].verts[11].z;
      }
      faces.add(quadInset);
      faces.add(new PVector[]{gon.hepts[i].vertsInset[8][0], gon.hepts[i].vertsInset[8][2], 
        quadInset[2], 
        quadInset[3]});
    }
    for (int i=0; i<6; i++) {
      int idx1 = i*2;
      int idx2 =(i*2+2) % 14;
      if (!faces.get(idx1)[3].equals(faces.get(idx2)[2])) {
        PVector midpoint = PVector.add(faces.get(idx1)[3], faces.get(idx2)[2]).div(2);
        faces.get(idx1)[3] = midpoint.copy();
        faces.get(idx2)[2] = midpoint.copy();
        faces.get(i*2+1)[3] = faces.get(idx1)[3];
        faces.get(i*2+3)[2] = faces.get(idx2)[2];
      }
    }
    center1 = pointOnEllipse(TWO_PI/7.f*7, eyesR, eyesR);
    center2 = pointOnEllipse(TWO_PI/7.f*7, eyesR, eyesR);
    outer1 = faces.get(0)[2].copy();
    outer2 = faces.get(12)[3].copy();
    center1.sub(new PVector(0, eyesD, 0));
    center2.add(new PVector(0, eyesD, 0));
    if (gon.insetWOZ) {
      outer1.z = 0;
      outer2.z = 0;
    }
    else {
      center2.z = 0;
      center1.z = 0;
    }
    PVector[] quad = {center2, center1, outer2, outer1};
    if (gon.insetWOZ) {
      quad[0].z = 0;
      quad[1].z = 0;
      quad[2].z = faces.get(12)[3].z;
      quad[3].z = faces.get(0)[2].z;
    }
    faces.add(quad);
  }
  
  void updateTypePyramids() {
    PVector center1, outer1, outer2;
    for (int i=0; i<7; i++) {
      outer1 = gon.hepts[i].verts[9].copy();
      outer2 = gon.hepts[i].verts[11].copy();
      if (i == 0 || i == 3 || i == 6) {
        center1 = new PVector(0, 0, 0);
        if (!gon.insetWOZ) {
          center1.z = z;
        }
      }
      else {
        // todo: eyes
        center1 = new PVector(0, 0, 0);
        if (i < 3) {
          center1.add(new PVector(0, eyesD, 0));
        }
        else {
          center1.sub(new PVector(0, eyesD, 0));
        }
      }
      if (gon.insetWOZ) {
        outer1.z = 0;
        outer2.z = 0;
      }
      PVector[] triInset = gon._insetTriangle(center1, 
        outer1, outer2,
        new PVector(0, 0), new PVector(0, 1), new PVector(1, 0));
      if (gon.insetWOZ) {
        if (i == 0 || i == 3 || i == 6) {
          triInset[0].z = z;
        }
        else {
          triInset[0].z = 0;
        }
        triInset[1].z = gon.hepts[i].verts[9].z;
        triInset[2].z = gon.hepts[i].verts[11].z;
      }
      faces.add(triInset);
      faces.add(new PVector[]{gon.hepts[i].vertsInset[8][0], gon.hepts[i].vertsInset[8][2], 
        triInset[1], 
        triInset[2]});
    }
    faces.add(new PVector[]{faces.get(0)[2], faces.get(2)[0], new PVector(0, 0, z)});
    faces.add(new PVector[]{faces.get(4)[2], faces.get(4)[0], new PVector(0, 0, z)});
    faces.add(new PVector[]{faces.get(8)[1], faces.get(8)[0], new PVector(0, 0, z)});
    faces.add(new PVector[]{faces.get(10)[2], faces.get(10)[0], new PVector(0, 0, z)});
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
  
  PShape getGlassCutOut(PVector[][] verts, boolean openTop, boolean openBottom) {
    PShape innerShape = createShape();
    if (openTop || openBottom) {
      innerShape.beginShape();
      PVector p = pointOnEllipse(0, diam, diam);
      if (openBottom) {
        innerShape.vertex(p.x, p.y);
        innerShape.vertex(verts[0][1].x, verts[0][1].y);
        if (!openTop) {
          if (type == Face.TYPE_ROBO || type == Face.TYPE_PYRAMIDS) {
            PVector i = intersection(verts[0][1], new PVector(0, eyesD), new PVector(0, -eyesD), verts[6][1], false);
            innerShape.vertex(i.x, i.y);
            
          }
          else {
            innerShape.vertex(0, 0);
          }
        }
        else {
          innerShape.vertex(0, type == Face.TYPE_ROBO || type == Face.TYPE_PYRAMIDS ? eyesD : 0);
        }
      }
      if (openTop) {
        PVector p1 = pointOnEllipse(TWO_PI/7.f*3, diam, diam);
        PVector p2 = pointOnEllipse(TWO_PI/7.f*4, diam, diam);
        innerShape.vertex(0, type == Face.TYPE_ROBO || type == Face.TYPE_PYRAMIDS ? eyesD : 0);
        innerShape.vertex(verts[2][1].x, verts[2][1].y);
        innerShape.vertex(p1.x, p1.y);
        innerShape.vertex(p2.x, p2.y);
        innerShape.vertex(verts[3][1].x, verts[3][1].y);
        innerShape.vertex(0, type == Face.TYPE_ROBO || type == Face.TYPE_PYRAMIDS ? -eyesD : 0);
      }
      if (openBottom) {
        innerShape.vertex(verts[6][1].x, verts[6][1].y);
        innerShape.vertex(p.x, p.y);
      }
      innerShape.endShape(CLOSE);
    }
    return innerShape;
  }
}
