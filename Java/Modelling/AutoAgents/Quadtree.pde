class Pack {
  PVector body;
  Drone store;

  Pack(Drone a) {
    body = a.pos.copy();
    store = a;
  }
}

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
  Pack[] data;
  boolean divided = false;

  // Children trees
  QuadTree[] children = new QuadTree[4];

  QuadTree(Rectangle boundary_, int n) {
    boundary = boundary_;
    data = new Pack[n];
  }

  void subdivide() {
    float north = boundary.y - boundary.h / 2;
    float east = boundary.x + boundary.w / 2;
    float south = boundary.y + boundary.h / 2;
    float west = boundary.x - boundary.w / 2;
    float new_width = boundary.w / 2;
    float new_height = boundary.h / 2;

    Rectangle ne = new Rectangle(east, north, new_width, new_height);
    children[0] = new QuadTree(ne, data.length);

    Rectangle nw = new Rectangle(west, north, new_width, new_height);
    children[1] = new QuadTree(nw, data.length);

    Rectangle se = new Rectangle(east, south, new_width, new_height);
    children[2] = new QuadTree(se, data.length);


    Rectangle sw = new Rectangle(west, south, new_width, new_height);
    children[3] = new QuadTree(sw, data.length);

    divided = true;
  }

  boolean insert(Drone a) {
    if (!boundary.contains(a.pos)) {
      return false;
    }

    if (occupancy < data.length) {
      data[occupancy] = new Pack(a);
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

  ArrayList query(Rectangle range, ArrayList<Drone> found) {
    if (!boundary.intersects(range)) {
      return found;
    } else {
      for (int i = 0; i < occupancy; i++) {
        if (range.contains(data[i].body)) {
          found.add(data[i].store);
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

  void debug(int verbosity) {
    switch(verbosity) {
    case 1:
      if (occupancy > 0) {
        drawBounds(true);
      }
      break;
    case 2:
      drawBounds(false);
      break;
    case 3:
      drawBounds(true);
      break;
    default:
      if (occupancy > 0) {
        drawBounds(false);
      }
      break;
    }

    if (divided) {
      children[0].debug(verbosity);
      children[1].debug(verbosity);
      children[2].debug(verbosity);
      children[3].debug(verbosity);
    }
  }

  void drawBounds(boolean drawpoints) {
    pushStyle();
    if (drawpoints) {
      if (occupancy > 0) {
        stroke(255);
        strokeWeight(3);
        for (int i = 0; i < occupancy; i++)
        {
          point(data[i].body.x, data[i].body.y);
        }
      }
    }

    rectMode(CENTER);
    stroke(255);
    strokeWeight(1);
    noFill();
    rect(boundary.x, boundary.y, boundary.w * 2, boundary.h * 2);
    popStyle();
  }
}