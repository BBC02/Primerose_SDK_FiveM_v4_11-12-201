#pragma once

namespace ESP
{
	void Boxes ( Entity& entity );
	void FilledBoxes ( Entity& entity );
	void Corners ( Entity& entity );
	void Snaplines ( ImVec2& src , Entity& entity , int& type , ImColor color );
	void Skeleton ( Entity& entity );
}
