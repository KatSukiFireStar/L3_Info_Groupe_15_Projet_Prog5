using System.Diagnostics;
using System.Reflection;

namespace Test;

public static class Utils
{
	public static readonly string PROJECT_PATH = Path.Combine(
		Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location), 
		"../../../../..");
	public static readonly string TEST_FILE = Path.Combine(PROJECT_PATH, "file1.o");
	public static readonly string PROGRAMM_PATH = Path.Combine(PROJECT_PATH, "elfFile");

	public static ICollection<string> ReadLines(this StreamReader reader)
	{
		List<string> result = new();

		while (reader.ReadLine() is { } line)
		{
			result.Add(line);
		}

		return result;
	}

	public static string ReadOutput(this Process p) => p.StandardOutput.ReadToEnd().Replace("\0", "");

	public static string Format(this string s) => s.Trim().ToLower();
	public static string[] Separate(this string s) => s.Split(':').Select(Format).ToArray();

	public static IEnumerable<(T value, int index)> EnumerateWithIndex<T>(this T[] array) => array.Select((t, i) => (t, i));
}