using System.Diagnostics;
using System.Globalization;

namespace Test;

[TestClass]
public class ElfDisplayTest
{
	private readonly ProcessStartInfo _elfFileStart;
	private Process? _elfFileProcess;

	private readonly ProcessStartInfo _readelfStart;
	private Process? _readelfProcess;
	private const string _HEADER_ARG = "-h";
	private const string _SECTION_ARG = "-S";

	public ElfDisplayTest()
	{
		_elfFileStart = new(Utils.PROGRAMM_PATH, Utils.TEST_FILE)
		{
			RedirectStandardError = true,
			RedirectStandardOutput = true,
			RedirectStandardInput = true
		};

		_readelfStart = new(@"readelf")
		{
			RedirectStandardError = true,
			RedirectStandardOutput = true,
			RedirectStandardInput = true
		};
	}

	private void ExecuteAndClose(char arg)
	{
		_elfFileProcess.StandardInput.WriteLine(arg);
		_elfFileProcess.StandardInput.WriteLine();
		_elfFileProcess.StandardInput.WriteLine('q');
	}

	private void SetArg(string arg)
	{
		_readelfStart.ArgumentList.Clear();
		_readelfStart.ArgumentList.Add(arg);
		_readelfStart.ArgumentList.Add(Utils.TEST_FILE);
	}

	[TestMethod]
	public void Header()
	{
		_elfFileProcess = Process.Start(_elfFileStart);

		_readelfStart.ArgumentList.Clear();
		_readelfStart.ArgumentList.Add(_HEADER_ARG);
		_readelfStart.ArgumentList.Add(Utils.TEST_FILE);
		_readelfProcess = Process.Start(_readelfStart);


		Assert.IsNotNull(_elfFileProcess);
		Assert.IsNotNull(_readelfProcess);

		ExecuteAndClose('h');

		/*
			Magic:   7f 45 4c 46 01 01 01 00 00 00 00 00 00 00 00 00
			Class:                             ELF32
			Data:                              2's complement, little endian
			Version:                           1 (current)
			OS/ABI:                            UNIX - System V
			ABI Version:                       0
			Type:                              REL (Relocatable file)
			Machine:                           Intel 80386
			Version:                           0x1
			Entry point address:               0x0
			Start of program headers:          0 (bytes into file)
			Start of section headers:          16100 (bytes into file)
			Flags:                             0x0
			Size of this header:               52 (bytes)
			Size of program headers:           0 (bytes)
			Number of program headers:         0
			Size of section headers:           40 (bytes)
			Number of section headers:         25
			Section header string table index: 24
		 */

		/*
			Magic:                        7f 45 4c 46 01 01 01 00 00 00 00 00 00 00 00 00
			Class:                        ELF32
			Data:                         2's complement, little endian
			Version:                      1 (Current)
			OS/ABI:                       UNIX System V ABI
			ABI Version:                  0
			Type:                         REL, Relocatable file
			Machine:                      Intel 80386
			Entry point address:          0x0
			Start of program headers:     0
			Start of section headers:     16100
			Flags:                        0x0 ( )
			Size of this header:          52 bytes
			Size of program headers:      0 bytes
			Number of program headers:    0
			Size of section headers:      40 bytes
			Number of section header:     25
			String table section index:   24
		 */

		Dictionary<string, string> keyReplacement = new()
		{
			["number of section header"] = "number of section headers",
			["string table section index"] = "section header string table index"
		};

		Dictionary<string, string> valueReplacement = new()
		{
			["unix system v abi"] = "unix - system v",
			["rel, relocatable file"] = "rel (relocatable file)"
		};

		Dictionary<string, string> readelfResult = _readelfProcess.StandardOutput.ReadLines()
			.Skip(1)
			.Select(Utils.Separate)
			.Where(l => !(l[0] == "version" && l[1].Contains("0x")))
			.Where(l => l.Length == 2)
			.ToDictionary(l => l[0], l => l[1]);
		Dictionary<string, string> elfFileResult = _elfFileProcess.StandardOutput.ReadLines()
			.Skip(9)
			.Take(18)
			.Select(Utils.Separate)
			.Where(l => l.Length == 2)
			.ToDictionary(l => keyReplacement.TryGetValue(l[0], out string key) ? key : l[0],
				l => valueReplacement.TryGetValue(l[1], out string value) ? value : l[1]);

		foreach ((string key, string value) in readelfResult)
		{
			string expectedValue = value;
			string actualValue = elfFileResult[key];

			switch (key)
			{
				case "start of program headers":
				case "start of section headers":
					expectedValue = expectedValue.Remove(expectedValue.IndexOf(" ("));
					break;
				case "flags":
					actualValue = actualValue.Remove(actualValue.IndexOf(" ("));
					break;
				case "size of this header":
				case "size of program headers":
				case "size of section headers":
					actualValue = actualValue.Replace("bytes", "(bytes)");
					break;
			}

			Assert.AreEqual(expectedValue, actualValue);
		}
	}

	[TestMethod]
	public void SectionTable()
	{
		_elfFileProcess = Process.Start(_elfFileStart);

		SetArg(_SECTION_ARG);
		_readelfProcess = Process.Start(_readelfStart);


		Assert.IsNotNull(_elfFileProcess);
		Assert.IsNotNull(_readelfProcess);

		ExecuteAndClose('s');

		/*
			Il y a 25 en-têtes de section, débutant à l'adresse de décalage 0x3ee4:

			En-têtes de section :
			  [Nr] Nom               Type            Adr      Décala.Taille ES Fan LN Inf Al
			  [ 0]                   NULL            00000000 000000 000000 00      0   0  0
			  [ 1] .group            GROUP           00000000 000034 000008 04     22  42  4
			  [ 2] .text             PROGBITS        00000000 00003c 000c5c 00  AX  0   0  1
			  [ 3] .rel.text         REL             00000000 002dd8 000610 08   I 22   2  4
			  [ 4] .data             PROGBITS        00000000 000c98 000000 00  WA  0   0  1
			  [ 5] .bss              NOBITS          00000000 000c98 000000 00  WA  0   0  1
			  [ 6] .rodata           PROGBITS        00000000 000c98 0007c0 00   A  0   0  4
			  [ 7] .rel.rodata       REL             00000000 0033e8 000538 08   I 22   6  4
			  [ 8] .text.__x86.[...] PROGBITS        00000000 001458 000004 00 AXG  0   0  1
			  [ 9] .debug_info       PROGBITS        00000000 00145c 00077b 00      0   0  1
			  [10] .rel.debug_info   REL             00000000 003920 000428 08   I 22   9  4
			  [11] .debug_abbrev     PROGBITS        00000000 001bd7 00024f 00      0   0  1
			  [12] .debug_aranges    PROGBITS        00000000 001e26 000020 00      0   0  1
			  [13] .rel.debug_a[...] REL             00000000 003d48 000010 08   I 22  12  4
			  [14] .debug_line       PROGBITS        00000000 001e46 0003b6 00      0   0  1
			  [15] .rel.debug_line   REL             00000000 003d58 000080 08   I 22  14  4
			  [16] .debug_str        PROGBITS        00000000 0021fc 0004ad 01  MS  0   0  1
			  [17] .debug_line_str   PROGBITS        00000000 0026a9 00012a 01  MS  0   0  1
			  [18] .comment          PROGBITS        00000000 0027d3 00001f 01  MS  0   0  1
			  [19] .note.GNU-stack   PROGBITS        00000000 0027f2 000000 00      0   0  1
			  [20] .eh_frame         PROGBITS        00000000 0027f4 0000d4 00   A  0   0  4
			  [21] .rel.eh_frame     REL             00000000 003dd8 000028 08   I 22  20  4
			  [22] .symtab           SYMTAB          00000000 0028c8 0003b0 10     23  41  4
			  [23] .strtab           STRTAB          00000000 002c78 00015e 00      0   0  1
			  [24] .shstrtab         STRTAB          00000000 003e00 0000e2 00      0   0  1
			Clé des fanions :
			  W (écriture), A (allocation), X (exécution), M (fusion), S (chaînes), I (info),
			  L (ordre des liens), O (traitement supplémentaire par l'OS requis), G (groupe),
			  T (TLS), C (compressé), x (inconnu), o (spécifique à l'OS), E (exclu),
			  D (mbind), p (processor specific)
		 */

		/*
			Section table: 
			Section 0
			  Section name:                         
			  Section type:                         0 ( Section header table entry unused )
			  Section address:                      0x0
			  Section offset:                       0x0
			  Section size:                         0x0
			  Entry size if section holds table:    0x0
			  Section flags:                        0x0 ( )
			  Link to another section:              0
			  Additional section information:       0
			  Section alignment (exponent of 2):    0

			Section 1
			  Section name:                         .group
			  Section type:                         17 (  )
			  Section address:                      0x0
			  Section offset:                       0x34
			  Section size:                         0x8
			  Entry size if section holds table:    0x4
			  Section flags:                        0x0 ( )
			  Link to another section:              22
			  Additional section information:       42
			  Section alignment (exponent of 2):    4

			Section 2
			  Section name:                         .text
			  Section type:                         1 ( Program data )
			  Section address:                      0x0
			  Section offset:                       0x3c
			  Section size:                         0xc5c
			  Entry size if section holds table:    0x0
			  Section flags:                        0x6 ( 'Alloc' 'Executable' )
			  Link to another section:              0
			  Additional section information:       0
			  Section alignment (exponent of 2):    1

			Section 3
			  Section name:                         .rel.text
			  Section type:                         9 ( Relocation entries, no addends )
			  Section address:                      0x0
			  Section offset:                       0x2dd8
			  Section size:                         0x610
			  Entry size if section holds table:    0x8
			  Section flags:                        0x40 ( )
			  Link to another section:              22
			  Additional section information:       2
			  Section alignment (exponent of 2):    4

			Section 4
			  Section name:                         .data
			  Section type:                         1 ( Program data )
			  Section address:                      0x0
			  Section offset:                       0xc98
			  Section size:                         0x0
			  Entry size if section holds table:    0x0
			  Section flags:                        0x3 ( 'Write' 'Alloc' )
			  Link to another section:              0
			  Additional section information:       0
			  Section alignment (exponent of 2):    1

			Section 5
			  Section name:                         .bss
			  Section type:                         8 ( Program space with no data (bss) )
			  Section address:                      0x0
			  Section offset:                       0xc98
			  Section size:                         0x0
			  Entry size if section holds table:    0x0
			  Section flags:                        0x3 ( 'Write' 'Alloc' )
			  Link to another section:              0
			  Additional section information:       0
			  Section alignment (exponent of 2):    1

			Section 6
			  Section name:                         .rodata
			  Section type:                         1 ( Program data )
			  Section address:                      0x0
			  Section offset:                       0xc98
			  Section size:                         0x7c0
			  Entry size if section holds table:    0x0
			  Section flags:                        0x2 ( 'Alloc' )
			  Link to another section:              0
			  Additional section information:       0
			  Section alignment (exponent of 2):    4

			Section 7
			  Section name:                         .rel.rodata
			  Section type:                         9 ( Relocation entries, no addends )
			  Section address:                      0x0
			  Section offset:                       0x33e8
			  Section size:                         0x538
			  Entry size if section holds table:    0x8
			  Section flags:                        0x40 ( )
			  Link to another section:              22
			  Additional section information:       6
			  Section alignment (exponent of 2):    4

			Section 8
			  Section name:                         .text.__x86.get_pc_thunk.bx
			  Section type:                         1 ( Program data )
			  Section address:                      0x0
			  Section offset:                       0x1458
			  Section size:                         0x4
			  Entry size if section holds table:    0x0
			  Section flags:                        0x206 ( 'Alloc' 'Executable' )
			  Link to another section:              0
			  Additional section information:       0
			  Section alignment (exponent of 2):    1

			Section 9
			  Section name:                         .debug_info
			  Section type:                         1 ( Program data )
			  Section address:                      0x0
			  Section offset:                       0x145c
			  Section size:                         0x77b
			  Entry size if section holds table:    0x0
			  Section flags:                        0x0 ( )
			  Link to another section:              0
			  Additional section information:       0
			  Section alignment (exponent of 2):    1

			Section 10
			  Section name:                         .rel.debug_info
			  Section type:                         9 ( Relocation entries, no addends )
			  Section address:                      0x0
			  Section offset:                       0x3920
			  Section size:                         0x428
			  Entry size if section holds table:    0x8
			  Section flags:                        0x40 ( )
			  Link to another section:              22
			  Additional section information:       9
			  Section alignment (exponent of 2):    4

			Section 11
			  Section name:                         .debug_abbrev
			  Section type:                         1 ( Program data )
			  Section address:                      0x0
			  Section offset:                       0x1bd7
			  Section size:                         0x24f
			  Entry size if section holds table:    0x0
			  Section flags:                        0x0 ( )
			  Link to another section:              0
			  Additional section information:       0
			  Section alignment (exponent of 2):    1

			Section 12
			  Section name:                         .debug_aranges
			  Section type:                         1 ( Program data )
			  Section address:                      0x0
			  Section offset:                       0x1e26
			  Section size:                         0x20
			  Entry size if section holds table:    0x0
			  Section flags:                        0x0 ( )
			  Link to another section:              0
			  Additional section information:       0
			  Section alignment (exponent of 2):    1

			Section 13
			  Section name:                         .rel.debug_aranges
			  Section type:                         9 ( Relocation entries, no addends )
			  Section address:                      0x0
			  Section offset:                       0x3d48
			  Section size:                         0x10
			  Entry size if section holds table:    0x8
			  Section flags:                        0x40 ( )
			  Link to another section:              22
			  Additional section information:       12
			  Section alignment (exponent of 2):    4

			Section 14
			  Section name:                         .debug_line
			  Section type:                         1 ( Program data )
			  Section address:                      0x0
			  Section offset:                       0x1e46
			  Section size:                         0x3b6
			  Entry size if section holds table:    0x0
			  Section flags:                        0x0 ( )
			  Link to another section:              0
			  Additional section information:       0
			  Section alignment (exponent of 2):    1

			Section 15
			  Section name:                         .rel.debug_line
			  Section type:                         9 ( Relocation entries, no addends )
			  Section address:                      0x0
			  Section offset:                       0x3d58
			  Section size:                         0x80
			  Entry size if section holds table:    0x8
			  Section flags:                        0x40 ( )
			  Link to another section:              22
			  Additional section information:       14
			  Section alignment (exponent of 2):    4

			Section 16
			  Section name:                         .debug_str
			  Section type:                         1 ( Program data )
			  Section address:                      0x0
			  Section offset:                       0x21fc
			  Section size:                         0x4ad
			  Entry size if section holds table:    0x1
			  Section flags:                        0x30 ( )
			  Link to another section:              0
			  Additional section information:       0
			  Section alignment (exponent of 2):    1

			Section 17
			  Section name:                         .debug_line_str
			  Section type:                         1 ( Program data )
			  Section address:                      0x0
			  Section offset:                       0x26a9
			  Section size:                         0x12a
			  Entry size if section holds table:    0x1
			  Section flags:                        0x30 ( )
			  Link to another section:              0
			  Additional section information:       0
			  Section alignment (exponent of 2):    1

			Section 18
			  Section name:                         .comment
			  Section type:                         1 ( Program data )
			  Section address:                      0x0
			  Section offset:                       0x27d3
			  Section size:                         0x1f
			  Entry size if section holds table:    0x1
			  Section flags:                        0x30 ( )
			  Link to another section:              0
			  Additional section information:       0
			  Section alignment (exponent of 2):    1

			Section 19
			  Section name:                         .note.GNU-stack
			  Section type:                         1 ( Program data )
			  Section address:                      0x0
			  Section offset:                       0x27f2
			  Section size:                         0x0
			  Entry size if section holds table:    0x0
			  Section flags:                        0x0 ( )
			  Link to another section:              0
			  Additional section information:       0
			  Section alignment (exponent of 2):    1

			Section 20
			  Section name:                         .eh_frame
			  Section type:                         1 ( Program data )
			  Section address:                      0x0
			  Section offset:                       0x27f4
			  Section size:                         0xd4
			  Entry size if section holds table:    0x0
			  Section flags:                        0x2 ( 'Alloc' )
			  Link to another section:              0
			  Additional section information:       0
			  Section alignment (exponent of 2):    4

			Section 21
			  Section name:                         .rel.eh_frame
			  Section type:                         9 ( Relocation entries, no addends )
			  Section address:                      0x0
			  Section offset:                       0x3dd8
			  Section size:                         0x28
			  Entry size if section holds table:    0x8
			  Section flags:                        0x40 ( )
			  Link to another section:              22
			  Additional section information:       20
			  Section alignment (exponent of 2):    4

			Section 22
			  Section name:                         .symtab
			  Section type:                         2 ( Symbol table )
			  Section address:                      0x0
			  Section offset:                       0x28c8
			  Section size:                         0x3b0
			  Entry size if section holds table:    0x10
			  Section flags:                        0x0 ( )
			  Link to another section:              23
			  Additional section information:       41
			  Section alignment (exponent of 2):    4

			Section 23
			  Section name:                         .strtab
			  Section type:                         3 ( String table )
			  Section address:                      0x0
			  Section offset:                       0x2c78
			  Section size:                         0x15e
			  Entry size if section holds table:    0x0
			  Section flags:                        0x0 ( )
			  Link to another section:              0
			  Additional section information:       0
			  Section alignment (exponent of 2):    1

			Section 24
			  Section name:                         .shstrtab
			  Section type:                         3 ( String table )
			  Section address:                      0x0
			  Section offset:                       0x3e00
			  Section size:                         0xe2
			  Entry size if section holds table:    0x0
			  Section flags:                        0x0 ( )
			  Link to another section:              0
			  Additional section information:       0
			  Section alignment (exponent of 2):    1
		 */

		string[][] readelfResult = _readelfProcess.StandardOutput.ReadLines()
			.Skip(4)
			.SkipLast(5)
			.Select((l, i) =>
			{
				string[] tmp = l.Replace("[ ", "[").Replace(" ]", "]")
					.Split(' ', StringSplitOptions.RemoveEmptyEntries | StringSplitOptions.TrimEntries)
					[1..];

				if (tmp.Length == 10)
					return tmp;


				if (tmp.Length != 10)
				{
					string[] result = new string[10];
					if (i == 0)
					{
						result[0] = "";
						Array.Copy(tmp, 0, result, 1, 5);
						result[6] = "";
						Array.Copy(tmp, 5, result, 7, 3);
					}
					else
					{
						Array.Copy(tmp, 0, result, 0, 6);
						result[6] = "";
						Array.Copy(tmp, 6, result, 7, 3);
					}

					return result;
				}

				return tmp;
			})
			.ToArray();

		for (int i = 0; i < 9; i++)
			_elfFileProcess.StandardOutput.ReadLine();

		string[][] elfFileResult = _elfFileProcess.StandardOutput.ReadToEnd()
			.Replace("\0", "")
			.Split("\n\n")
			.SkipLast(2)
			.Select(l => l.Split("\n", StringSplitOptions.TrimEntries)[1..]
				.Select(ll => ll.Split(":", StringSplitOptions.TrimEntries)[1]).ToArray())
			.ToArray();

		Dictionary<string, string> typeReplacements = new()
		{
			["NULL"] = "0",
			["PROGBITS"] = "1",
			["SYMTAB"] = "2",
			["STRTAB"] = "3",
			["RELA"] = "4",
			["HASH"] = "5",
			["DYNAMIC"] = "6",
			["NOTE"] = "7",
			["NOBITS"] = "8",
			["REL"] = "9",
			["SHLIB"] = "10",
			["DYNSYM"] = "11",
			["INIT_ARRAY"] = "14",
			["FINI_ARRAY"] = "15",
			["PREINIT_ARRAY"] = "16",
			["GROUP"] = "17",
			["SYMTAB_SHNDX"] = "18",
			["RELR"] = "19",
			["NUM"] = "20"
		};

		Dictionary<char, int> flags = new()
		{
			['W'] = (1 << 0),
			['A'] = (1 << 1),
			['X'] = (1 << 2),
			['M'] = (1 << 4),
			['S'] = (1 << 5),
			['I'] = (1 << 6),
			['L'] = (1 << 7),
			['O'] = (1 << 8),
			['G'] = (1 << 9),
			['T'] = (1 << 10),
			['C'] = (1 << 11),
			// ['E'] = (1U << 31)
		};

		foreach ((string[] section, int i) in readelfResult.EnumerateWithIndex())
			foreach ((string value, int j) in section.EnumerateWithIndex())
			{
				string expectedValue = value;
				string actualValue = elfFileResult[i][j];

				switch (j)
				{
					case 0:
						if (!expectedValue.Contains("[...]"))
							break;

						expectedValue = expectedValue[..expectedValue.IndexOf("[...]")];
						actualValue = actualValue[..expectedValue.Length];
						break;
					case 1:
						expectedValue = typeReplacements[expectedValue];
						actualValue = actualValue.Remove(actualValue.IndexOf(" ("));
						break;

					case 2 or 3 or 4 or 5:
						expectedValue = Convert.ToInt32(expectedValue, 16).ToString();
						actualValue = Convert.ToInt32(actualValue, 16).ToString();
						break;

					case 6:
						int tmp = expectedValue.Aggregate(0x0, (current, flag) => current | flags[flag]);
						expectedValue = tmp.ToString();
						actualValue = actualValue.Remove(actualValue.IndexOf(" ("))[2..];
						actualValue = Convert.ToInt32(actualValue, 16).ToString();
						break;
				}

				Assert.AreEqual(expectedValue, actualValue);
			}

		/*foreach ((string key, string value) in readelfResult)
		{
			string expectedValue = value;
			string actualValue = elfFileResult[key];

			switch (key)
			{
				case "Type":
					actualValue = actualValue.Replace(" (", ", ");
					actualValue = actualValue.Remove(elfFileResult[key].IndexOf(")"), 1);
					break;
				case "start of program headers":
				case "start of section headers":
					expectedValue = expectedValue.Remove(expectedValue.IndexOf(" ("));
					break;
				case "flags":
					actualValue = actualValue.Remove(actualValue.IndexOf(" ("));
					break;
				case "size of this header":
				case "size of program headers":
				case "size of section headers":
					actualValue = actualValue.Replace("bytes", "(bytes)");
					break;
			}

			Assert.AreEqual(expectedValue, actualValue);
		}*/
	}
}