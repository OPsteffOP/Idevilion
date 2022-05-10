float4 main() : SV_TARGET
{
#if defined(GRID_COLOR)
	return float4(0.25f, 0.25f, 0.25f, 1.f);
#elif defined(CHUNK_COLOR)
	return float4(0.75f, 0.75f, 0.75f, 1.f);
#elif defined(HIGHLIGHT_COLOR)
	return float4(1.f, 0.f, 0.f, 1.f);
#else
	return float4(1.f, 1.f, 1.f, 1.f);
#endif
}