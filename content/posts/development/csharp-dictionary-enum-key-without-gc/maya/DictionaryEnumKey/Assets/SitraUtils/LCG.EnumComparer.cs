using System;
using System.Collections.Generic;
using System.Reflection.Emit;
using System.Runtime.CompilerServices;

[assembly:InternalsVisibleTo("SitraUtils.Benchmarks")]
namespace SitraUtils.LCG
{
    public static class EnumComparer
    {
        public static EnumComparer<TEnum> For<TEnum>()
            where TEnum : struct, IComparable, IConvertible, IFormattable
        {
            return EnumComparer<TEnum>.Instance;
        }
    }

    /// <summary>
    /// A fast and efficient implementation of <see cref="IEqualityComparer{T}"/> for Enum types.
    /// Useful for dictionaries that use Enums as their keys.
    /// </summary>
    /// <example>
    /// <code>
    /// var dict = new Dictionary&lt;DayOfWeek, string&gt;(EnumComparer&lt;DayOfWeek&gt;.Instance);
    /// </code>
    /// </example>
    /// <typeparam name="TEnum">The type of the Enum.</typeparam>
    public sealed class EnumComparer<TEnum> : IEqualityComparer<TEnum>
        where TEnum : struct, IComparable, IConvertible, IFormattable
    {
        private readonly Func<TEnum, TEnum, bool> equals;
        private readonly Func<TEnum, int> getHashCode;

        /// <summary>
        /// The singleton accessor.
        /// </summary>
        public static readonly EnumComparer<TEnum> Instance;

        /// <summary>
        /// Initializes the <see cref="EnumComparer{TEnum}"/> class
        /// by generating the GetHashCode and Equals methods.
        /// </summary>
        static EnumComparer()
        {
            assertTypeIsEnum();
            assertUnderlyingTypeIsSupported();
            Instance = new EnumComparer<TEnum>();
        }

        /// <summary>
        /// A private constructor to prevent user instantiation.
        /// </summary>
        internal EnumComparer()
        {
            getHashCode = generateGetHashCode();
            equals = generateEquals();
        }

        /// <summary>
        /// Determines whether the specified objects are equal.
        /// </summary>
        /// <param name="x">The first object of type <typeparamref name="TEnum"/> to compare.</param>
        /// <param name="y">The second object of type <typeparamref name="TEnum"/> to compare.</param>
        /// <returns>
        /// true if the specified objects are equal; otherwise, false.
        /// </returns>
        public bool Equals(TEnum x, TEnum y)
        {
            // call the generated method
            return equals(x, y);
        }

        /// <summary>
        /// Returns a hash code for the specified object.
        /// </summary>
        /// <param name="obj">The <see cref="T:System.Object"/> for which a hash code is to be returned.</param>
        /// <returns>A hash code for the specified object.</returns>
        /// <exception cref="T:System.ArgumentNullException">
        /// The type of <paramref name="obj"/> is a reference type and <paramref name="obj"/> is null.
        /// </exception>
        public int GetHashCode(TEnum obj)
        {
            // call the generated method
            return getHashCode(obj);
        }

        private static void assertTypeIsEnum()
        {
            if (typeof(TEnum).IsEnum)
                return;

            var message =
                string.Format("The type parameter {0} is not an Enum. LcgEnumComparer supports Enums only.",
                              typeof(TEnum));
            throw new NotSupportedException(message);
        }

        private static void assertUnderlyingTypeIsSupported()
        {
            var underlyingType = Enum.GetUnderlyingType(typeof(TEnum));
            ICollection<Type> supportedTypes =
                new[]
                    {
                        typeof (byte), typeof (sbyte), typeof (short), typeof (ushort),
                        typeof (int), typeof (uint), typeof (long), typeof (ulong)
                    };

            if (supportedTypes.Contains(underlyingType))
                return;

            var message =
                string.Format("The underlying type of the type parameter {0} is {1}. " +
                              "LcgEnumComparer only supports Enums with underlying type of " +
                              "byte, sbyte, short, ushort, int, uint, long, or ulong.",
                              typeof(TEnum), underlyingType);
            throw new NotSupportedException(message);
        }

        /// <summary>
        /// Generates a comparison method similiar to this:
        /// <code>
        /// bool Equals(TEnum x, TEnum y)
        /// {
        ///     return x == y;
        /// }
        /// </code>
        /// </summary>
        /// <returns>The generated method.</returns>
        private static Func<TEnum, TEnum, bool> generateEquals_static()
        {
            var method = new DynamicMethod(typeof(TEnum).Name + "_Equals",
                                           typeof(bool),
                                           new[] { typeof(EnumComparer<TEnum>), typeof(TEnum), typeof(TEnum) },
                                           typeof(EnumComparer<TEnum>), true);
            var generator = method.GetILGenerator();
            // Writing body
            generator.Emit(OpCodes.Ldarg_0);    // load x to stack
            generator.Emit(OpCodes.Ldarg_1);    // load y to stack
            generator.Emit(OpCodes.Ceq);        // x == y
            generator.Emit(OpCodes.Ret);        // return result

            return (Func<TEnum, TEnum, bool>)method.CreateDelegate(typeof(Func<TEnum, TEnum, bool>));
        }
        /// <summary>
        /// Generates a comparison method similiar to this:
        /// <code>
        /// bool Equals(TEnum x, TEnum y)
        /// {
        ///     return x == y;
        /// }
        /// </code>
        /// </summary>
        /// <returns>The generated method.</returns>
        private Func<TEnum, TEnum, bool> generateEquals()
        {
            /*
                        var method = new DynamicMethod(typeof(TEnum).Name + "_Equals",
                                                       typeof(bool),
                                                       new[] { typeof(EnumComparer<TEnum>), typeof(TEnum), typeof(TEnum) },
                                                       typeof(EnumComparer<TEnum>), true);
            */
            var method = new DynamicMethod(typeof(TEnum).Name + "_Equals",
                                           typeof(bool),
                                           new[] { typeof(EnumComparer<TEnum>), typeof(TEnum), typeof(TEnum) },
                                           true);
            var generator = method.GetILGenerator();
            // Writing body
            generator.Emit(OpCodes.Ldarg_1);    // load x to stack
            generator.Emit(OpCodes.Ldarg_2);    // load y to stack
            generator.Emit(OpCodes.Ceq);        // x == y
            generator.Emit(OpCodes.Ret);        // return result

            return (Func<TEnum, TEnum, bool>)method.CreateDelegate(typeof(Func<TEnum, TEnum, bool>), this);
        }

        /// <summary>
        /// Generates a GetHashCode method similar to this:
        /// <code>
        /// int GetHashCode(TEnum obj)
        /// {
        ///     return ((int)obj).GetHashCode();
        /// }
        /// </code>
        /// </summary>
        /// <returns>The generated method.</returns>
        private Func<TEnum, int> generateGetHashCode()
        {
            /*
                        var method = new DynamicMethod(typeof(TEnum).Name + "_GetHashCode",
                                                       typeof(int),
                                                       new[] { typeof(EnumComparer<TEnum>), typeof(TEnum) },
                                                       typeof(EnumComparer<TEnum>), true);
            */
            var method = new DynamicMethod(typeof(TEnum).Name + "_GetHashCode",
                                           typeof(int),
                                           new[] { typeof(EnumComparer<TEnum>), typeof(TEnum) },
                                           typeof(EnumComparer<TEnum>), true);
            var generator = method.GetILGenerator();

            var underlyingType = Enum.GetUnderlyingType(typeof(TEnum));
            var getHashCodeMethod = underlyingType.GetMethod("GetHashCode");

            var castValue = generator.DeclareLocal(underlyingType);
            // Writing body
            generator.Emit(OpCodes.Ldarg_1);                    // load obj to stack
            generator.Emit(OpCodes.Conv_I4);                    // cast
            generator.Emit(OpCodes.Stloc, castValue);                    // castValue = obj
            generator.Emit(OpCodes.Ldloca, castValue);        // load *castValue to stack
            //generator.Emit(OpCodes.Stloc_0);                    // castValue = obj
            //generator.Emit(OpCodes.Ldloca_S, castValue);        // load *castValue to stack
            generator.Emit(OpCodes.Call, getHashCodeMethod);    // castValue.GetHashCode()
            generator.Emit(OpCodes.Ret);                        // return result
            return (Func<TEnum, int>)method.CreateDelegate(typeof(Func<TEnum, int>), this);
        }
        /// <summary>
        /// Generates a GetHashCode method similar to this:
        /// <code>
        /// int GetHashCode(TEnum obj)
        /// {
        ///     return ((int)obj).GetHashCode();
        /// }
        /// </code>
        /// </summary>
        /// <returns>The generated method.</returns>
        private static Func<TEnum, int> generateGetHashCode_static()
        {
            var method = new DynamicMethod(typeof(TEnum).Name + "_GetHashCode",
                                           typeof(int),
                                           new[] { typeof(EnumComparer<TEnum>), typeof(TEnum) },
                                           typeof(EnumComparer<TEnum>), true);
            var generator = method.GetILGenerator();

            var underlyingType = Enum.GetUnderlyingType(typeof(TEnum));
            var getHashCodeMethod = underlyingType.GetMethod("GetHashCode");

            var castValue = generator.DeclareLocal(underlyingType);
            // Writing body
            generator.Emit(OpCodes.Ldarg_0);                    // load obj to stack
            generator.Emit(OpCodes.Conv_I4);                    // cast
            generator.Emit(OpCodes.Stloc, castValue);                    // castValue = obj
            generator.Emit(OpCodes.Ldloca, castValue);        // load *castValue to stack
            //generator.Emit(OpCodes.Stloc_0);                    // castValue = obj
            //generator.Emit(OpCodes.Ldloca_S, castValue);        // load *castValue to stack
            generator.Emit(OpCodes.Call, getHashCodeMethod);    // castValue.GetHashCode()
            generator.Emit(OpCodes.Ret);                        // return result
            return (Func<TEnum, int>)method.CreateDelegate(typeof(Func<TEnum, int>));
        }
    }
    /// <summary>
    /// A fast and efficient implementation of <see cref="IEqualityComparer{T}"/> for Enum types.
    /// Useful for dictionaries that use Enums as their keys.
    /// </summary>
    /// <example>
    /// <code>
    /// var dict = new Dictionary&lt;DayOfWeek, string&gt;(EnumComparer&lt;DayOfWeek&gt;.Instance);
    /// </code>
    /// </example>
    /// <typeparam name="TEnum">The type of the Enum.</typeparam>
    public sealed class EnumComparer2<TEnum> : IEqualityComparer<TEnum>
        where TEnum : struct, IComparable, IConvertible, IFormattable
    {
        private readonly Func<TEnum, TEnum, bool> equals;
        private readonly Func<TEnum, int> getHashCode;

        /// <summary>
        /// The singleton accessor.
        /// </summary>
        public static readonly EnumComparer2<TEnum> Instance;

        /// <summary>
        /// Initializes the <see cref="EnumComparer{TEnum}"/> class
        /// by generating the GetHashCode and Equals methods.
        /// </summary>
        static EnumComparer2()
        {
            Instance = new EnumComparer2<TEnum>();
        }

        /// <summary>
        /// A private constructor to prevent user instantiation.
        /// </summary>
        internal EnumComparer2()
        {
            getHashCode = generateGetHashCode();
            equals = generateEquals();
            assertTypeIsEnum();
            assertUnderlyingTypeIsSupported();
        }

        /// <summary>
        /// Determines whether the specified objects are equal.
        /// </summary>
        /// <param name="x">The first object of type <typeparamref name="TEnum"/> to compare.</param>
        /// <param name="y">The second object of type <typeparamref name="TEnum"/> to compare.</param>
        /// <returns>
        /// true if the specified objects are equal; otherwise, false.
        /// </returns>
        public bool Equals(TEnum x, TEnum y)
        {
            // call the generated method
            return equals(x, y);
        }

        /// <summary>
        /// Returns a hash code for the specified object.
        /// </summary>
        /// <param name="obj">The <see cref="T:System.Object"/> for which a hash code is to be returned.</param>
        /// <returns>A hash code for the specified object.</returns>
        /// <exception cref="T:System.ArgumentNullException">
        /// The type of <paramref name="obj"/> is a reference type and <paramref name="obj"/> is null.
        /// </exception>
        public int GetHashCode(TEnum obj)
        {
            // call the generated method
            return getHashCode(obj);
        }

        private static void assertTypeIsEnum()
        {
            if (typeof(TEnum).IsEnum)
                return;

            var message =
                string.Format("The type parameter {0} is not an Enum. LcgEnumComparer supports Enums only.",
                              typeof(TEnum));
            throw new NotSupportedException(message);
        }

        private static void assertUnderlyingTypeIsSupported()
        {
            var underlyingType = Enum.GetUnderlyingType(typeof(TEnum));
            ICollection<Type> supportedTypes =
                new[]
                    {
                        typeof (byte), typeof (sbyte), typeof (short), typeof (ushort),
                        typeof (int), typeof (uint), typeof (long), typeof (ulong)
                    };

            if (supportedTypes.Contains(underlyingType))
                return;

            var message =
                string.Format("The underlying type of the type parameter {0} is {1}. " +
                              "LcgEnumComparer only supports Enums with underlying type of " +
                              "byte, sbyte, short, ushort, int, uint, long, or ulong.",
                              typeof(TEnum), underlyingType);
            throw new NotSupportedException(message);
        }

        /// <summary>
        /// Generates a comparison method similiar to this:
        /// <code>
        /// bool Equals(TEnum x, TEnum y)
        /// {
        ///     return x == y;
        /// }
        /// </code>
        /// </summary>
        /// <returns>The generated method.</returns>
        private Func<TEnum, TEnum, bool> generateEquals()
        {
            /*
                        var method = new DynamicMethod(typeof(TEnum).Name + "_Equals",
                                                       typeof(bool),
                                                       new[] { typeof(EnumComparer<TEnum>), typeof(TEnum), typeof(TEnum) },
                                                       typeof(EnumComparer<TEnum>), true);
            */
            var method = new DynamicMethod(typeof(TEnum).Name + "_Equals",
                                           typeof(bool),
                                           new[] { typeof(EnumComparer2<TEnum>), typeof(TEnum), typeof(TEnum) },
                                           true);
            var generator = method.GetILGenerator();
            // Writing body
            generator.Emit(OpCodes.Ldarg_1);    // load x to stack
            generator.Emit(OpCodes.Ldarg_2);    // load y to stack
            generator.Emit(OpCodes.Ceq);        // x == y
            generator.Emit(OpCodes.Ret);        // return result

            return (Func<TEnum, TEnum, bool>)method.CreateDelegate(typeof(Func<TEnum, TEnum, bool>), this);
        }

        /// <summary>
        /// Generates a GetHashCode method similar to this:
        /// <code>
        /// int GetHashCode(TEnum obj)
        /// {
        ///     return ((int)obj).GetHashCode();
        /// }
        /// </code>
        /// </summary>
        /// <returns>The generated method.</returns>
        private Func<TEnum, int> generateGetHashCode()
        {
            /*
                        var method = new DynamicMethod(typeof(TEnum).Name + "_GetHashCode",
                                                       typeof(int),
                                                       new[] { typeof(EnumComparer<TEnum>), typeof(TEnum) },
                                                       typeof(EnumComparer<TEnum>), true);
            */
            var method = new DynamicMethod(typeof(TEnum).Name + "_GetHashCode",
                                           typeof(int),
                                           new[] { typeof(EnumComparer2<TEnum>), typeof(TEnum) },
                                           typeof(EnumComparer<TEnum>), true);
            var generator = method.GetILGenerator();

            var underlyingType = Enum.GetUnderlyingType(typeof(TEnum));
            var getHashCodeMethod = underlyingType.GetMethod("GetHashCode");

            var castValue = generator.DeclareLocal(underlyingType);
            // Writing body
            generator.Emit(OpCodes.Ldarg_1);                    // load obj to stack
            generator.Emit(OpCodes.Conv_I4);                    // cast
            generator.Emit(OpCodes.Stloc, castValue);                    // castValue = obj
            generator.Emit(OpCodes.Ldloca, castValue);        // load *castValue to stack
            //generator.Emit(OpCodes.Stloc_0);                    // castValue = obj
            //generator.Emit(OpCodes.Ldloca_S, castValue);        // load *castValue to stack
            generator.Emit(OpCodes.Call, getHashCodeMethod);    // castValue.GetHashCode()
            generator.Emit(OpCodes.Ret);                        // return result
            return (Func<TEnum, int>)method.CreateDelegate(typeof(Func<TEnum, int>), this);
        }
    }
}