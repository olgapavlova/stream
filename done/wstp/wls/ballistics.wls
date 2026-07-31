v0 = 50;
g = 9.81;

angles = {5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 80, 80} Degree;

plots = Table[
  theta = angles[[i]];

  Plot[
    x Tan[theta] - g x^2/(2 v0^2 Cos[theta]^2),
    {x, 0, v0^2 Sin[2 theta]/g},
    PlotStyle -> ColorData[97][i]
  ],
  {i, Length[angles]}
];

image = Show[
  plots,
  PlotRange -> All,
  AxesLabel -> {"x (m)", "y (m)"},
  GridLines -> Automatic,
  ImageSize -> Large,
	PlotLabel -> "Ballistic trajectories"
];

Export["ballistics.png", image, "PNG"]
