using System.Diagnostics;

namespace Test;

public static class Utils
{
	public const string PROJECT_PATH = "../../../../..";
	public const string TEST_FILE = $"{PROJECT_PATH}/file1.o";
	public const string PROGRAMM_PATH = $"{PROJECT_PATH}/elfFile";

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