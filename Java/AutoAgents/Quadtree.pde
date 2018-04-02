class Rectangle {
  float x, y, w, h;
  Rectangle(float x_, float y_, float w_, float h_) {
    x = x_;
    y = y_;
    w = w_;
    h = h_;
  }

  boolean contains(PVector a) {
    return ((a.x >= x - w) &&
      (a.x <= x + w) &&
      (a.y >= y - h) &&
      (a.y <= y + h));
  }

  boolean intersects(Rectangle range) {
    return !((range.x - range.w > x + w) ||
      (range.x + range.w < x - w) ||
      (range.y - range.h > y + h) ||
      (range.y + range.h < y - h));
  }
}

class QuadTree {
  Rectangle boundary;
  int occupancy = 0;
  PVector[] points;
  boolean divided = false;

  // Children trees
  QuadTree[] children = new QuadTree[4];

  QuadTree(Rectangle boundary_, int n) {
    boundary = boundary_;
    points = new PVector[n];
  }

  void subdivide() {
    float north = boundary.y - boundary.h / 2;
    float east = boundary.x + boundary.w / 2;
    float south = boundary.y + boundary.h / 2;
    float west = boundary.x - boundary.w / 2;
    float new_width = boundary.w / 2;
    float new_height = boundary.h / 2;

    Rectangle ne = new Rectangle(east, north, new_width, new_height);
    children[0] = new QuadTree(ne, points.length);

    Rectangle nw = new Rectangle(west, north, new_width, new_height);
    children[1] = new QuadTree(nw, points.length);

    Rectangle se = new Rectangle(east, south, new_width, new_height);
    children[2] = new QuadTree(se, points.length);


    Rectangle sw = new Rectangle(west, south, new_width, new_height);
    children[3] = new QuadTree(sw, points.length);

    divided = true;
  }

  boolean insert(PVector a) {
    if (!boundary.contains(a)) {
      return false;
    }

    if (occupancy < points.length) {
      points[occupancy] = a.copy();
      occupancy++;
      return true;
    } else {
      if (!divided) {
        subdivide();
      }
      if (children[0].insert(a) ||
        children[1].insert(a) ||
        children[2].insert(a) ||
        children[3].insert(a)) {
        return true;
      } else {
        return false;
      }
    }
  }

  ArrayList query(Rectangle range, ArrayList<PVector> found) {
    if (!boundary.intersects(range)) {
      return found;
    } else {
      for (int i = 0; i < occupancy; i++) {
        if (range.contains(points[i])) {
          found.add(points[i]);
        }
      }
      if (divided) {
        children[0].query(range, found); 
        children[1].query(range, found);
        children[2].query(range, found);
        children[3].query(range, found);
      }
      return found;
    }
  }

  void debug() {
    pushStyle();
    rectMode(CENTER);
    stroke(255);
    strokeWeight(1);
    noFill();
    rect(boundary.x, boundary.y, boundary.w * 2, boundary.h * 2);

    for (int i = 0; i < occupancy; i++) {
      stroke(255);
      strokeWeight(3);
      point(points[i].x, points[i].y);
    }

    if (divided) {
      children[0].debug();
      children[1].debug();
      children[2].debug();
      children[3].debug();
    }
    popStyle();
  }
}
