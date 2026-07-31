ClearAll[maxHeightAndAngle];

maxHeightAndAngle[v0_?NumericQ, l_?NumericQ, g_: 9.81] :=
    Module[
        {height, angle},

        If[v0 <= 0 || l < 0 || g <= 0,
            Return[$Failed]
        ];

        height =
            v0^2/(2 g) -
            g l^2/(2 v0^2);
				angle =
						If[
								l == 0,
								90,
								N[ArcTan[v0^2/(g l)]/Degree]
						];

        N[{height, angle}]
    ];
