class CodedKeys {
  boolean ctrlKey;
  boolean shiftKey;
  
  void keyPressed() {
    if (key == CODED) {
      if (keyCode == CONTROL) {
        ctrlKey = true;
      }
      else if (keyCode == SHIFT) {
        shiftKey = true;
      }
    }
  }
  
  void keyReleased() {
    if (key == CODED) {
      if (keyCode == CONTROL) {
        ctrlKey = false;
      }
      else if (keyCode == SHIFT) {
        shiftKey = false;
      }
    }
  }
}
