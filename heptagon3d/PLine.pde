class PLine {
  PVector a;
  PVector b;
  
  PLine(PVector a, PVector b) {
    this.a = a;
    this.b = b;
  }
  
  PVector intersection(PLine other, boolean checkLengths)
  {
    // center everything on point "d"
    PVector axis = PVector.sub(other.a, other.b);
    float axLen = axis.mag();
    axis.normalize();
    PVector workingA = PVector.sub(a, other.b);
    PVector workingB = PVector.sub(b, other.b);
  
    // create a perpendicular vector to "c-d"
    PVector rightang = new PVector(-axis.y, axis.x);
  
    // In short: rotate everything so "c-d" becomes the y-axis
    //   rightang becomes x-axis
    PVector mappedA = new PVector(workingA.dot(rightang), workingA.dot(axis));
    PVector mappedB = new PVector(workingB.dot(rightang), workingB.dot(axis));
    // More detail: mappedA and -B are projections of "a" and "b" onto the lines
    //   "c-d" and "rightang", creating Axis Aligned 2D coordinates
  
    // Get the axis-aligned segment
    PVector dir = mappedA.sub(mappedB);
  
    // This is the same math used for 2D axis-aligned-bounding-boxes but only used
    //   for one intersection instead of two edges
    // In other words:
    //   "How much do we change segment 'a-b's length to reach segment 'c-d'?"
    // Result can be +/- INF, meaning segments are parallel
    // Relying on the floating point to handle div by 0.0 --> INF
    //   is implementation dependant. Your hardware may vary.
    float DIV_TOO_FAR = 0.00001;
    float tx = 1.0 / DIV_TOO_FAR;
    if (abs(dir.x) > DIV_TOO_FAR)
      tx = -mappedB.x / dir.x;
    
    // when the original line segment "a-b" is extended/shortened by tx,
    //   the end of that segment is the intersecting point
    PVector inters = PVector.sub(a, b).mult(tx).add(b);
    if (checkLengths) {
      PVector result = null;
      // Segment/segment intersection:
      // Logic is that if the first segment would have to expand or reverse to
      //   reach the point at 'inters', then the segments do not cross
      float ty = PVector.sub(inters, other.b).dot(axis);
      if ((tx >= 0) && (tx <= 1.0) && (ty >= 0) && (ty <= axLen)) {
        result = inters;
      }
      return result;
    }
    return inters;
  }  
}
