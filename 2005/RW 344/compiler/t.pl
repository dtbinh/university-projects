(*
output
10
2
1
2
115
1
10
115
1
*)


begin
  integer x, y;
  boolean f;
	proc a(integer y;integer value pi;boolean bo)
	begin 
		write y,pi,bo;
		(*pi:=244;*)
		y:=pi;
	end;
	x:=10;
	y:=2;
	f:=true;
	write x,y,f;
	a(y,115,f);
	write x,y,f;
end.
