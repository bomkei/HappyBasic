#pragma once

static constexpr auto FUNC_CALL_DEPTH_MAX = 100;

class Options
{
public:
  static std::string FileName;
  static bool Compile;
  static bool IsSafety;

};