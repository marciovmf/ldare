
struct Point
{
  int dx, dy;
  int euclideanDist() const { return dx*dx + dy*dy; }
};

const Point BLACK = {0, 0};
const Point WHITE = {9999, 9999};

struct Grid
{
  uint32 width;
  uint32 height;
  Point* grid;

  Grid(uint32 width, uint32 height)
    :width(width), height(height)
  {
    grid = new Point[width * height];
  }

  ~Grid()
  {
    delete grid;
  }
};

Point get(Grid &g, int x, int y)
{
  const int32 width = g.width;
  const int32 height = g.height;

  if (x >= 0 && y >= 0 && x < width && y < height)
    return g.grid[y * g.width + x];
  else
    return WHITE;
}

void put(Grid &g, int x, int y, const Point &p)
{
  g.grid[y * g.width + x] = p;
}

void compare(Grid &g, Point &p, int x, int y, int offsetx, int offsety)
{
  Point other = get(g, x+offsetx, y+offsety);
  other.dx += offsetx;
  other.dy += offsety;

  if (other.euclideanDist() < p.euclideanDist())
    p = other;
}

void GenerateSDF(Grid &g)
{
  const int32 width = g.width;
  const int32 height = g.height;

  // Top-left pass
  for (int y=height-1; y>=0; y--)
  {
    for (int x=0; x < width; x++)
    {
      Point p = get(g, x, y);
      compare(g, p, x, y, -1, 0);
      compare(g, p, x, y,  0, 1);
      compare(g, p, x, y, -1, 1);
      compare(g, p, x, y,  1, 1);
      put(g, x, y, p);
    }

    for (int x=width-1; x>=0; x--)
    {
      Point p = get(g, x, y);
      compare(g, p, x, y, 1, 0);
      put(g, x, y, p);
    }
  }

  // Bottom-right pass
  for (int y=0; y < height; y++)
  {
    for (int x=width-1; x>=0; x--)
    {
      Point p = get(g, x, y);
      compare(g, p, x, y,  1,  0);
      compare(g, p, x, y,  0, -1);
      compare(g, p, x, y, -1, -1);
      compare(g, p, x, y,  1, -1);
      put(g, x, y, p);
    }

    for (int x=0; x<width; x++)
    {
      Point p = get(g, x, y);
      compare(g, p, x, y, -1, 0);
      put(g, x, y, p);
    }
  }
}

