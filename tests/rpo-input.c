int
foo(int a)
{
	if (a == 0)
	{
		return 1;
	}
	return 0;
}

int
main(int argc, char **argv)
{
	if (argc < 2)
	{
		argc++;
	}
	while (argc > 5)
	{
		argc--;
	}
	return 0;
}
